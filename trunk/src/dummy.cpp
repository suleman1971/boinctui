#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/md5.h>
#include "srvdata.h"


Srv* connect1;


int main()
{
    const char* req0 = "<boinc_gui_rpc_request>\n<get_state/></boinc_gui_rpc_request>\n\003";
    const char* req1 = "<boinc_gui_rpc_request>\n<get_results>\n<active_only>1</active_only>\n</get_results></boinc_gui_rpc_request>\n\003";
    const char* req2 = "<boinc_gui_rpc_request>\n<get_simple_gui_info/></boinc_gui_rpc_request>\n\003";
    const char* req3 = "<boinc_gui_rpc_request>\n<get_all_projects_list/></boinc_gui_rpc_request>\n\003";
    const char* req4 = "<boinc_gui_rpc_request>\n<get_disk_usage/></boinc_gui_rpc_request>\n\003";
    const char* req5 = "<boinc_gui_rpc_request>\n<get_statistics/></boinc_gui_rpc_request>\n\003";
    const char* req6 = "<boinc_gui_rpc_request>\n<get_cc_status/></boinc_gui_rpc_request>\n\003";
    const char* req7 = "<boinc_gui_rpc_request>\n<network_available/></boinc_gui_rpc_request>\n\003";
    const char* req8 = "<boinc_gui_rpc_request>\n<get_screensaver_tasks/></boinc_gui_rpc_request>\n\003";
    const char* req9 = "<boinc_gui_rpc_request>\n<run_benchmarks/></boinc_gui_rpc_request>\n\003";
    const char* req10= "<boinc_gui_rpc_request>\n<get_proxy_settings/></boinc_gui_rpc_request>\n\003";
    const char* req11= "<boinc_gui_rpc_request>\n<get_message_count/></boinc_gui_rpc_request>\n\003";
    const char* req12= "<boinc_gui_rpc_request>\n<get_messages>\n<seqno>3</seqno>\n</boinc_gui_rpc_request>\n\003";
    const char* req13= "<boinc_gui_rpc_request>\n<get_host_info/>\n</boinc_gui_rpc_request>\n\003";
    const char* req14= "<boinc_gui_rpc_request>\n<quit/>\n</boinc_gui_rpc_request>\n\003"; //ЗАВЕРШАЕТ ДЕМОНА !!!
    const char* req15= "<boinc_gui_rpc_request>\n<acct_mgr_rpc_poll/>\n</boinc_gui_rpc_request>\n\003";
    const char* req16= "<boinc_gui_rpc_request>\n<acct_mgr_info/>\n</boinc_gui_rpc_request>\n\003";
    const char* req17= "<boinc_gui_rpc_request>\n<get_project_init_status/>\n</boinc_gui_rpc_request>\n\003";
    const char* req18= "<boinc_gui_rpc_request>\n<get_project_config_poll/>\n</boinc_gui_rpc_request>\n\003";
    const char* req19= "<boinc_gui_rpc_request>\n<lookup_account_poll/>\n</boinc_gui_rpc_request>\n\003";

//    void* hconnect = openconnect("192.168.1.98","31416");
    connect1 = new Srv("192.168.1.98","31416","pass123");
    connect1->login();

    connect1->sendreq(req0);
    char* result = connect1->waitresult();
    printf("%s\nlen=%d\n", result, strlen(result));
    free(result); //если раз-т больше не нужен
/*
    connect1->sendreq(req4);
    result = connect1->waitresult();
    printf("%s\nlen=%d\n", result, strlen(result));
    free(result); //если раз-т больше не нужен
*/
}