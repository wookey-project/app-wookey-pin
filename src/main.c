/**
 * @file main.c
 *
 * \brief Main of dummy
 *
 */

#include "api/syscall.h"
#include "api/print.h"
#include "libusart.h"
#include "libconsole.h"
#include "libshell.h"
#include "ipc_proto.h"
#include "autoconf.h"

static void my_irq_handler(void);

void cb_usart_irq(uint32_t sr, uint32_t dr)
{
    sr = sr;
    dr = dr;
    // nothing done for input data from USART...
}

/************************************
 * IPC Communication protocol, basics
 ***********************************/


/*
 * We use the local -fno-stack-protector flag for main because
 * the stack protection has not been initialized yet.
 */
int _main(uint32_t task_id)
{
    uint8_t id_smart;
    uint8_t id;
    uint8_t ret;
    struct sync_command ipc_sync_cmd;

    printf("Hello ! I'm pin, my id is %x\n", task_id);

#ifdef CONFIG_APP_PIN_INPUT_USART
    ret = shell_early_init(CONFIG_APP_PIN_INPUT_USART_ID, 115200);
    if (ret != SYS_E_DONE) {
        printf("ERROR: registering USART through libshell failed.\n");
        while (1)
            ;
    }
    printf("Registered USART through libshell %d\n", ret);
#endif

    ret = sys_init(INIT_GETTASKID, "smart", &id_smart);
    if (ret != SYS_E_DONE) {
      printf("gettaskid fails with %s\n", strerror(ret));
    } else {
      printf("gettaskid ends with %s\n", strerror(ret));
      printf("smart is task %x !\n", id_smart);
    }

    printf("set init as done\n");
    ret = sys_init(INIT_DONE);
    printf("sys_init returns %s !\n", strerror(ret));

    /*******************************************
     * let's syncrhonize with other tasks
     *******************************************/
    logsize_t size = 2;

    printf("sending end_of_init syncrhonization to smart\n");
    ipc_sync_cmd.magic = MAGIC_TASK_STATE_CMD;
    ipc_sync_cmd.state = SYNC_READY;

    do {
        ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
    } while (ret != SYS_E_DONE);

    /* Now wait for Acknowledge from Smart */
    id = id_smart;

    do {
        ret = sys_ipc(IPC_RECV_SYNC, &id, &size, (char*)&ipc_sync_cmd);
    } while (ret != SYS_E_DONE);
    if (   ipc_sync_cmd.magic == MAGIC_TASK_STATE_RESP
        && ipc_sync_cmd.state == SYNC_ACKNOWLEDGE) {
        printf("smart has acknowledge end_of_init, continuing\n");
    }

    /*******************************************
     * End of full task end_of_init synchronization
     *******************************************/

    char * pin = 0;

#ifdef CONFIG_APP_PIN_INPUT_USART
    shell_init();
    if (ret == 0) {
        printf("USART4 is now configured !\n");
    } else {
        printf("error during configuration of USART4\n");
    }
    console_log("[USART4] Pin initialized usart...\n");
    console_flush();
#endif


    /*******************************************
     * Wait for smart to ask for PIN code
     * (when Smartcard is connected)
     *******************************************/
    uint32_t pin_len = 0;

    id = id_smart;
    size = 2;
    do {
        ret = sys_ipc(IPC_RECV_SYNC, &id, &size, (char*)&ipc_sync_cmd);
    } while (ret != SYS_E_DONE);

    if (   ipc_sync_cmd.magic == MAGIC_CRYPTO_PIN_CMD
        && ipc_sync_cmd.state == SYNC_ASK_FOR_DATA) {
        printf("smart is asking for pin, asking user through UART...\n");
    }

#ifdef CONFIG_APP_PIN_INPUT_USART
    console_log("Enter pin code please\n");
    console_flush();
    shell_readline(&pin, &pin_len); /*FIXME: update API, set string... and size */
    console_log("pin is %s\n", pin);
    printf("pin len is %x\n", pin_len);
    console_flush();
#endif

    ipc_sync_cmd.magic = MAGIC_CRYPTO_PIN_RESP;
    ipc_sync_cmd.state = SYNC_DONE;
    ipc_sync_cmd.data_size = (uint8_t)pin_len;
    memset(&ipc_sync_cmd.data, 0x0, 32);
    memcpy(&ipc_sync_cmd.data, pin, pin_len);

    do {
        size = 3 + pin_len;
        ret = sys_ipc(IPC_SEND_SYNC, id_smart, size, (char*)&ipc_sync_cmd);
    } while (ret != SYS_E_DONE);
    printf("Pin sent to smart\n");


    /* avoid exiting main thread */
    while (1) {
       sys_yield();
    }
    /* should return to do_endoftask() */
    return 0;
}
