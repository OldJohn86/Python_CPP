#ifdef HAVE_TK_OAM
#include "oam_cmn.h"
#include "oam_core.h"
#include "oam_client.h"
#include "oam_mux_par.h"
#include "oam_adapt.h"

#include "oam_tk.h"
#include "oam_tk_pdu.h"
#include "oam_tk_event.h"
#include "oam_tk_adapt.h"

#ifdef HAVE_KT_OAM
#include "oam_kt_upg.h"
#include "oam_kt_event.h"
#endif

#ifdef HAVE_DPOE_OAM

tk_oam_xlat_rountine_t g_tk_dpoe_oam_xlat_table[] =
{
#undef xx
#define xx(SEQ,BRANCH,LEAF,LEAF_VAL,GET_FUNC,SET_FUNC)   {BRANCH,LEAF,GET_FUNC,SET_FUNC},
    TK_DPOE_OAM_BUILD_RESP
};

/* TK OAM Queue config 07/1FF1 start */

oam_uint8 g_tk_oam_queue_cfg_len = 0;
oam_uint8 g_tk_oam_queue_cfg_resp[80];

oam_uint32 tk_oam_queue_get(
    tk_oam_mgmt_obj_t mgmtObj,
    oam_uint8 branch,
    oam_uint16 leaf,
    oam_uint8 *pOut )
{
    oam_uint32 pdu_len = OFFSET_LEN;
    //tk_oam_queue_cfg_t *data = (tk_oam_queue_cfg_t *)pOut;
    
    memcpy(pOut, &g_tk_oam_queue_cfg_resp[0], g_tk_oam_queue_cfg_len);

    pdu_len += g_tk_oam_queue_cfg_len;

    return pdu_len;
}

oam_uint32 tk_oam_queue_set(
    oam_uint8 *pRecv,
    oam_uint32 *inLen,
    tk_oam_mgmt_obj_t mgmtObj,
    oam_uint32 *outPktLen,
    oam_uint8 *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_uint32 pdu_len = OFFSET_LEN;
    oam_var_cont_t *data = (oam_var_cont_t *)pOut;
    tk_oam_subtype_header_t *queue = (tk_oam_subtype_header_t *)pRecv;
    oam_uint8 subtype;
    tk_oam_subtype_terminator_t *terminator;
    tk_oam_subtype_header_t *header;
    tk_oam_subtype_link_t *link;
    tk_oam_subtype_port_t *port;    
    oam_uint8 report_mode;
    static oam_uint8 llid_num = 0;
    static oam_uint8 port_num = 0;
    
    /* parse to get llid_num and uni num */
    subtype = queue->subtype;
    OAM_ORG_DEBUG("port %d, subtype %d, report_mode %d\r\n", mgmtObj.port, subtype, queue->report_mode);

    if(subtype == TK_OAM_SUBTYPE_HEADER){
        
        header = (tk_oam_subtype_header_t *)pRecv;
        report_mode = header->report_mode;
    #if 0      
        OAM_ORG_DEBUG("\r\nreport_mode %d\r\n", report_mode);
        switch(report_mode){
        case TK_OAM_REPORT_MODE_FRAME_ALIGNED_LINK16:
            g_tk_oam_link_num = 16;
            break;
        case TK_OAM_REPORT_MODE_MULTI_PRI3_LINK8:
            g_tk_oam_link_num = 8;
            break;
        case TK_OAM_REPORT_MODE_MULTI_PRI4_LINK6:
            g_tk_oam_link_num = 6;
            break;
        case TK_OAM_REPORT_MODE_MULTI_PRI8_LINK3:
            g_tk_oam_link_num = 3;
            break;
        default:
            data->width = TK_VAR_RSP_CODE_BAD_PARAM;
            return sizeof(oam_var_cont_t);
        }
    #endif
        llid_num = 0;
        ret = tk_oam_queue_report_mode_set_adapt(report_mode);
        
        g_tk_oam_queue_cfg_len = 0;
        memset(&g_tk_oam_queue_cfg_resp[0], 0, sizeof(g_tk_oam_queue_cfg_resp));
        memcpy(&g_tk_oam_queue_cfg_resp[g_tk_oam_queue_cfg_len], pRecv, sizeof(tk_oam_subtype_header_t));
        g_tk_oam_queue_cfg_len += sizeof(tk_oam_subtype_header_t);
        
    }else if(subtype == TK_OAM_SUBTYPE_LINK){
        link = (tk_oam_subtype_link_t *)pRecv;
        ret = tk_oam_link_queue_set_adapt(llid_num, link->link_queue);
        llid_num++;
        
        memcpy(&g_tk_oam_queue_cfg_resp[g_tk_oam_queue_cfg_len], pRecv, sizeof(oam_var_cont_t) + queue->hdr.width);
        g_tk_oam_queue_cfg_len += sizeof(oam_var_cont_t) + queue->hdr.width;
        
    }else if(subtype == TK_OAM_SUBTYPE_PORT){ 
        port = (tk_oam_subtype_port_t *)pRecv;
        ret = tk_oam_port_queue_set_adapt(port_num+1, port->port_queue);
        port_num++;
        
        memcpy(&g_tk_oam_queue_cfg_resp[g_tk_oam_queue_cfg_len], pRecv, sizeof(oam_var_cont_t) + queue->hdr.width);
        g_tk_oam_queue_cfg_len += sizeof(oam_var_cont_t) + queue->hdr.width;
        
    }if(subtype == TK_OAM_SUBTYPE_TERMINATOR){
        
        memcpy(&g_tk_oam_queue_cfg_resp[g_tk_oam_queue_cfg_len], pRecv, sizeof(oam_var_cont_t) + queue->hdr.width);
        g_tk_oam_queue_cfg_len += sizeof(oam_var_cont_t) + queue->hdr.width;
        
        /* restart other llid regtration if it is MLLID mode */
        if(llid_num > 1){

            tk_oam_mllid_reg_enable_adapt(llid_num);
        }
    } 
    
    data->width = TK_VAR_RSP_CODE_OK;
    return pdu_len;
}

/* TK OAM 07/1FF1 end */


oam_uint32 tk_oam_port_mtu_get(
    tk_oam_mgmt_obj_t mgmtObj,
    oam_uint8 branch,
    oam_uint16 leaf,
    oam_uint8 *pOut )
{
    oam_status ret = OAM_E_OK;
    oam_uint32 pdu_len = OFFSET_LEN;
    oam_var_cont_val_16_t *data = (oam_var_cont_val_16_t *)pOut;
    oam_uint16 port = mgmtObj.port;
    oam_uint16 mtu = 0;

    ret = tk_oam_port_mtu_get_adapt(port, &mtu);
    data->hdr.width = sizeof(oam_uint16);
    data->value = htons(mtu);
    pdu_len += sizeof(oam_uint16);

    return pdu_len;
}

oam_uint32 tk_oam_port_mtu_set(
    oam_uint8 *pRecv,
    oam_uint32 *inLen,
    tk_oam_mgmt_obj_t mgmtObj,
    oam_uint32 *outPktLen,
    oam_uint8 *pOut)
{
    oam_status ret = OAM_E_OK;
    oam_uint32 pdu_len = OFFSET_LEN;
    tk_oam_var_cont_t *data = (tk_oam_var_cont_t *)pOut;
    oam_var_cont_val_16_t *in = (oam_var_cont_val_16_t *)pRecv;
    oam_uint16 port = mgmtObj.port;
    oam_uint16 mtu = ntohs(in->value);

    OAM_ORG_LOG("mtu_set port %d, mtu %d\r\n", port, mtu);
    ret = tk_oam_port_mtu_set_adapt(port, mtu);

    data->len = TK_VAR_RSP_CODE_OK;
    
    return pdu_len;
}
    
#endif

#ifdef HAVE_KT_OAM

/******************************************************************************/
/*                           TK MSG                                        */
/******************************************************************************/

oam_uint8 g_oam_tk_port_stats_get = 0;

/*Extern global variables*/
extern oam_uint8 g_flow_control[4];
oam_uint8 g_auto_nego[4] = {1, 1, 1, 1};
oam_uint8 g_uni_enable[4] = {1, 1, 1, 1};

static oam_uint32
tk_oam_onu_port_statis_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    oam_uint64 tmp_value = 0;
    oam_status ret = OAM_E_OK;
    static tk_oam_onu_port_statis_data_t tk_statis;

    tmp_value = 0;

    if(!g_oam_tk_port_stats_get)
    {
        g_oam_tk_port_stats_get = 1;
        ret = tk_oam_port_statistics_get_adapt(mgmtObj.port,&tk_statis);
    }
    else
        ret = OAM_E_OK;


    if(OAM_E_OK == ret)
    {
        if(branch == TK_OAM_BRANCH_ATTRIB)
        {
            switch(leaf)
            {
                /* ONU PORT RX */
                case TK_OAM_LEAF_RX_UNICAST_FRAMES:
                    memcpy(&tmp_value, &tk_statis.in_unicast, sizeof(tk_statis.in_unicast));
                    break;
                case TK_OAM_ATTRIB_LEAF_OCTETS_RX_OK:
                    memcpy(&tmp_value, &tk_statis.in_octets, sizeof(oam_uint64));
                    break;
                case TK_OAM_ATTRIB_LEAF_FRAMES_RX_OK:
                    tmp_value = tk_statis.in_unicast + tk_statis.in_multicast + tk_statis.in_broadcast;
                    break;
                case TK_OAM_ATTRIB_LEAF_MC_RX_COUNT:
                    memcpy(&tmp_value, &tk_statis.in_multicast, sizeof(tk_statis.in_multicast));
                    break;
                case TK_OAM_ATTRIB_LEAF_BC_RX_COUNT:
                    memcpy(&tmp_value, &tk_statis.in_broadcast, sizeof(tk_statis.in_broadcast));
                    break;
                case TK_OAM_LEAF_RX_64_FRAMES:
                    memcpy(&tmp_value, &tk_statis.in_64_octets, sizeof(tk_statis.in_64_octets));
                    break;
                case TK_OAM_LEAF_RX_65_127_FRAMES:
                    memcpy(&tmp_value, &tk_statis.in_65to127_octets, sizeof(tk_statis.in_65to127_octets));
                    break;
                case TK_OAM_LEAF_RX_128_255_FRAMES:
                    memcpy(&tmp_value, &tk_statis.in_128to255_octets, sizeof(tk_statis.in_128to255_octets));
                    break;
                case TK_OAM_LEAF_RX_256_511_FRAMES:
                    memcpy(&tmp_value, &tk_statis.in_256to511_octets, sizeof(tk_statis.in_256to511_octets));
                    break;
                case TK_OAM_LEAF_RX_512_1023_FRAMES:
                    memcpy(&tmp_value, &tk_statis.in_512to1023_octets, sizeof(tk_statis.in_512to1023_octets));
                    break;
                case TK_OAM_LEAF_RX_1024_1518_FRAMES:
                    memcpy(&tmp_value, &tk_statis.in_1024t01518_octets, sizeof(tk_statis.in_1024t01518_octets));
                    break;
                case TK_OAM_LEAF_RX_1519_PLUS_FRAMES:
                    memcpy(&tmp_value, &tk_statis.in_1519_plus_octets, sizeof(tk_statis.in_1519_plus_octets));
                    break;
                case TK_OAM_LEAF_RX_TOO_SHORT_FRAMES:
                    memcpy(&tmp_value, &tk_statis.in_too_short_octets, sizeof(tk_statis.in_too_short_octets));
                    break;
                case TK_OAM_ATTRIB_LEAF_FRAME_TOO_LONG:
                    memcpy(&tmp_value, &tk_statis.frame_too_longs, sizeof(tk_statis.frame_too_longs));
                    break;
                case TK_OAM_ATTRIB_LEAF_MAC_CTRL_PAUSE_RX:
                    memcpy(&tmp_value, &tk_statis.in_pause_frames, sizeof(tk_statis.in_pause_frames));
                    break;
                case TK_OAM_ATTRIB_LEAF_IN_RANGE_LEN_ERROR:
                    tmp_value = 0;
                    break;
                case TK_OAM_ATTRIB_LEAF_ALIGNMENT_ERROR:
                    memcpy(&tmp_value, &tk_statis.alignment_errors, sizeof(tk_statis.alignment_errors));
                    break;
                case TK_OAM_ATTRIB_LEAF_FCS_ERROR:
                    memcpy(&tmp_value, &tk_statis.fcs_errors, sizeof(tk_statis.fcs_errors));
                    break;


                /* ONU PORT TX */
                case TK_OAM_ATTRIB_LEAF_OCTETS_TX_OK:
                    memcpy(&tmp_value, &tk_statis.out_octets, sizeof(tk_statis.out_octets));
                    break;
                case TK_OAM_ATTRIB_LEAF_FRAME_TX_OK:
                    tmp_value = tk_statis.out_unicast + tk_statis.out_multicast + tk_statis.out_broadcast;
                    break;
                case TK_OAM_LEAF_TX_UNICAST_FRAMES:
                    memcpy(&tmp_value, &tk_statis.out_unicast, sizeof(tk_statis.out_unicast));
                    break;
                case TK_OAM_ATTRIB_LEAF_MC_TX_COUNT:
                    memcpy(&tmp_value, &tk_statis.out_multicast, sizeof(tk_statis.out_multicast));
                    break;
                case TK_OAM_ATTRIB_LEAF_BC_TX_COUNT:
                    memcpy(&tmp_value, &tk_statis.out_broadcast, sizeof(tk_statis.out_broadcast));
                    break;
                case TK_OAM_LEAF_TX_64_FRAMES:
                    memcpy(&tmp_value, &tk_statis.out_64_octets, sizeof(tk_statis.out_64_octets));
                    break;
                case TK_OAM_LEAF_TX_65_127_FRAMES:
                    memcpy(&tmp_value, &tk_statis.out_65to127_octets, sizeof(tk_statis.out_65to127_octets));
                    break;
                case TK_OAM_LEAF_TX_128_255_FRAMES:
                    memcpy(&tmp_value, &tk_statis.out_128to255_octets, sizeof(tk_statis.out_128to255_octets));
                    break;
                case TK_OAM_LEAF_TX_256_511_FRAMES:
                    memcpy(&tmp_value, &tk_statis.out_256to511_octets, sizeof(tk_statis.out_256to511_octets));
                    break;
                case TK_OAM_LEAF_TX_512_1023_FRAMES:
                    memcpy(&tmp_value, &tk_statis.out_512to1023_octets, sizeof(tk_statis.out_512to1023_octets));
                    break;
                case TK_OAM_LEAF_TX_1024_1518_FRAMES:
                    memcpy(&tmp_value, &tk_statis.out_1024t01518_octets, sizeof(tk_statis.out_1024t01518_octets));
                    break;
                case TK_OAM_LEAF_TX_1519_PLUS_FRAMES:
                    memcpy(&tmp_value, &tk_statis.out_1519_plus_octets, sizeof(tk_statis.out_1519_plus_octets));
                    break;
                case TK_OAM_ATTRIB_LEAF_MAC_CTRL_PAUSE_TX:
                    memcpy(&tmp_value, &tk_statis.out_pause_frames, sizeof(tk_statis.out_pause_frames));
                    break;
                case TK_OAM_LEAF_TX_DROPPED_BYTES:
                    tmp_value = 0;
                    break;
                case TK_OAM_LEAF_TX_DROPPED_FRAMES:
                    tmp_value = 0;
                    break;
                case TK_OAM_ATTRIB_LEAF_SINGLE_COLLISION_FRAME:
                    memcpy(&tmp_value, &tk_statis.single_collision, sizeof(tk_statis.single_collision));
                    break;
                case TK_OAM_ATTRIB_LEAF_MULTIPLE_COLLISION_FRAME:
                    memcpy(&tmp_value, &tk_statis.multiple_collision, sizeof(tk_statis.multiple_collision));
                    break;
                case TK_OAM_ATTRIB_LEAF_LATE_COLLISIONS:
                    memcpy(&tmp_value, &tk_statis.late_collisions, sizeof(tk_statis.late_collisions));
                    break;
                case TK_OAM_ATTRIB_LEAF_EXCESSIVE_COLLISIONS:
                    memcpy(&tmp_value, &tk_statis.excessive_collisions, sizeof(tk_statis.excessive_collisions));
                    break;

                /* ONU LINK RX */
                case TK_OAM_LEAF_RX_MPCP_GATE:
                    memcpy(&tmp_value, &tk_statis.in_mpcp_gate, sizeof(tk_statis.in_mpcp_gate));
                    break;
                case TK_OAM_LEAF_RX_MPCP_REGISTER:
                    memcpy(&tmp_value, &tk_statis.in_mpcp_register, sizeof(tk_statis.in_mpcp_register));
                    break;

                /* ONU LINK TX */
                case TK_OAM_LEAF_TX_MPCP_REPORT:
                    memcpy(&tmp_value, &tk_statis.out_mpcp_report, sizeof(tk_statis.out_mpcp_report));
                    break;
                case TK_OAM_LEAF_TX_MPCP_REG_REQUEST:
                    memcpy(&tmp_value, &tk_statis.out_mpcp_reg_request, sizeof(tk_statis.out_mpcp_reg_request));
                    break;
                case TK_OAM_LEAF_TX_MPCP_REG_ACK:
                    memcpy(&tmp_value, &tk_statis.out_mpcp_reg_ack, sizeof(tk_statis.out_mpcp_reg_ack));
                    break;


                /* STATS not used by ubiquos OLT */
                case TK_OAM_ATTRIB_LEAF_FRAMES_DEFERRED:
                    memcpy(&tmp_value, &tk_statis.deferred_transmissions, sizeof(tk_statis.deferred_transmissions));
                    break;
                case TK_OAM_ATTRIB_LEAF_LOST_MAC_TX_ERROR:
                    memcpy(&tmp_value, &tk_statis.internal_mac_tx_errors, sizeof(tk_statis.internal_mac_tx_errors));
                    break;
                case TK_OAM_ATTRIB_LEAF_LOST_MAC_RX_ERROR:
                    memcpy(&tmp_value, &tk_statis.internal_mac_rx_errors, sizeof(tk_statis.internal_mac_rx_errors));
                    break;
                case TK_OAM_ATTRIB_LEAF_PON_SEN_ERR:
                    memcpy(&tmp_value, &tk_statis.carrier_sense_errors, sizeof(tk_statis.carrier_sense_errors));
                    break;


                /* ONU PORT RX specific for EPON port */
                case TK_OAM_LEAF_RX_DELAY_THRESHOLD:
                case TK_OAM_LEAF_RX_DELAY:
                case TK_OAM_LEAF_RX_DROPPED_FRAMES:
                case TK_OAM_LEAF_RX_DROPPED_BYTES:
                case TK_OAM_LEAF_RX_DELAYED_BYTES:
                case TK_OAM_ATTRIB_LEAF_PHY_ERR:
                case TK_OAM_LEAF_RX_ECS_EVENT:

                /* ONU PORT TX specific for EPON port */
                case TK_OAM_LEAF_TX_DELAY_THRESHOLD:
                case TK_OAM_LEAF_TX_DELAY:
                case TK_OAM_LEAF_TX_DELAYED_BYTES:
                case TK_OAM_LEAF_TX_UNUSED_BYTES:

                /* ONU LINK RX */
                case TK_OAM_LEAF_RX_MAC_CTRL_FRAMES:
                case TK_OAM_LEAF_RX_ONU_HOLDOVER:
                case TK_OAM_LEAF_RX_SLE_MODE:

                /* ONU LINK TX */
                case TK_OAM_LEAF_TX_MAC_CTRL_FRAMES:


                case TK_OAM_ATTRIB_LEAF_14:
                case TK_OAM_ATTRIB_LEAF_10:
                case TK_OAM_ATTRIB_LEAF_11:
                case TK_OAM_ATTRIB_LEAF_RSTP_MODE:

                    break;

                default:
                    p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;
                    OAM_ORG_LOG( "\r\nunknow TK leaf under attrib branch 0x%x\r\n", ntohs(leaf));
                    return len;
                    break;

            }

            /* special handle for 1bytes and 2bytes counters 009b, 0095 */
            if(leaf == TK_OAM_LEAF_RX_DELAY_THRESHOLD || leaf == TK_OAM_LEAF_TX_DELAY_THRESHOLD)
            {
                p_rsp_tlv->len = 1;
                p_rsp_tlv->data[0] = tmp_value;

            }else if(leaf == TK_OAM_LEAF_RX_ONU_HOLDOVER) /* 00e6 */
            {
                tmp_value = htons(tmp_value);
                p_rsp_tlv->len = sizeof(oam_uint16);
                memcpy(p_rsp_tlv->data, &tmp_value, sizeof(oam_uint16));

            }else
            {
                tmp_value = htonll(tmp_value);
                p_rsp_tlv->len = sizeof(oam_uint64);
                memcpy(p_rsp_tlv->data, &tmp_value, sizeof(oam_uint64));

            }
            len += p_rsp_tlv->len;
        }

    }
    else{
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;
    }

    return len;
}

static oam_uint32
tk_oam_dync_mac_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_mac_rules_t rules[TK_MAX_MAC_PER_PORT];
    oam_boolean first = FALSE;
    oam_uint8 mac_num = 0;
    oam_boolean flag = TRUE;
    oam_uint16 i = 0;

#ifndef HAVE_MPORTS
    if(mgmtObj.port != OAM_UNI_PORT_ID1)
        flag = FALSE;
#endif

    if(flag)
    {
        memset(rules, 0, sizeof(tk_oam_mac_rules_t));
        tk_oam_dync_mac_get_adapt(mgmtObj.port, rules, &mac_num);
        OAM_ORG_LOG( "\r\ndync mac num %d\r\n", mac_num);
    }

    if(mac_num > 0)
    {
        for(i = 0; i<mac_num; i ++)
        {
            p_rsp_tlv->len = sizeof(tk_oam_mac_rules_t);

            /*if(retCode)
                break;
            if(len>= OAM_MAX_OAMPDU_SIZE - sizeof(oam_pdu_kt_index_t) + 2 - sizeof(tk_oam_pdu_t))
                break;*/

            if(!first)
                {
                    first = TRUE;
                    memcpy(p_rsp_tlv->data, &rules[i], sizeof(tk_oam_mac_rules_t));
                    len += sizeof(tk_oam_mac_rules_t);
                }
            else
                {
                    pOut += sizeof(tk_oam_tlv_t) - 1 + sizeof(tk_oam_mac_rules_t);
                    p_rsp_tlv = (tk_oam_tlv_t *)pOut;
                    p_rsp_tlv->branch = branch;
                    p_rsp_tlv->leaf = htons(leaf);
                    p_rsp_tlv->len = sizeof(tk_oam_mac_rules_t);
                    memcpy(p_rsp_tlv->data, &rules[i], sizeof(tk_oam_mac_rules_t));
                    len += sizeof(tk_oam_tlv_t) - 1 + sizeof(tk_oam_mac_rules_t);
                }
        }
    }

    if(first){
        pOut += sizeof(tk_oam_tlv_t) - 1 + sizeof(tk_oam_mac_rules_t);
        p_rsp_tlv = (tk_oam_tlv_t *)pOut;
        p_rsp_tlv->branch = branch;
        p_rsp_tlv->leaf = htons(leaf);
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
        len += sizeof(tk_oam_tlv_t) - 1;
    }
    else{
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    }

    return len;
}

static oam_uint32
tk_oam_onu_rule_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = 0;
    oam_uint32 i = 0;
    oam_uint32 aal_rc = 0;
    oam_uint32 num_rules = 8;
    oam_int32 rule_size = 0;
    epon_tk_rule_t rsp;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    oam_uint8 *pOutPoint = pOut;

    OAM_ORG_LOG( "\r\ntk onu rule get\r\n");

    if (ntohs(leaf) == TK_OAM_ATTRIB_LEAF_ONU_DOWN_RULE)
        num_rules = TK_DS_RULES_MAX_NUM;
    else if (ntohs(leaf) == TK_OAM_ATTRIB_LEAF_ONU_UP_RULE)
        num_rules = TK_US_RULES_MAX_NUM;

    tk_rule_getindex_clr_adapt();

    for(i = 0; i < num_rules; i++) {
        memset(&rsp, 0, sizeof(epon_tk_rule_t));
        if(tk_oam_onu_rules_get_adapt(mgmtObj.port, (oam_uint8 *)&rsp, &rule_size) != OAM_E_OK)
        {
            aal_rc = 1;
            break;
        }
        p_rsp_tlv->branch = branch;
        p_rsp_tlv->leaf = htons(leaf);
        p_rsp_tlv->len = rule_size;
        memcpy(p_rsp_tlv->data, &rsp, rule_size);
        len += sizeof(tk_oam_tlv_t) - 1 + rule_size;
        pOutPoint += len;
        p_rsp_tlv = (tk_oam_tlv_t *)pOutPoint;
    }


    if(!aal_rc)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len - sizeof(tk_oam_var_cont_t);
}

static oam_uint32
tk_oam_report_threshold_set(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_report_threshold_set_t *p_set = NULL;
    oam_uint16 tmp_value = 0;
    oam_uint16 tk_report_threshold = 0;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    if (p_tlv->len != sizeof(tk_oam_report_threshold_set_t))
    {    OAM_ORG_LOG( "\r\ninv len %d %d\r\n", (oam_uint32)p_tlv->len, (oam_uint32)sizeof(tk_oam_report_threshold_set_t)); }
    else
    {
        p_set = (tk_oam_report_threshold_set_t *)p_tlv->data;
        tmp_value = (oam_uint16) (p_set->threshold & 0xffff);
        tk_report_threshold = ntohs(tmp_value);

        OAM_ORG_LOG( "\r\nthreshold 0x%x\r\n", tk_report_threshold);
        retCode = tk_oam_report_threshold_set_adapt(tk_report_threshold);
        if(!retCode)
            p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
        else
            p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;
    }

    return len;
}

static oam_uint32
tk_oam_report_threshold_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint32 tk_report_threshold = 0;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    oam_status  retCode = OAM_E_OK;

    retCode = tk_oam_report_threshold_get_adapt((oam_uint16 *)&tk_report_threshold);
    OAM_ORG_LOG( "\r\ntk report threshold 0x%x\r\n", tk_report_threshold);
    tk_report_threshold = htonl(tk_report_threshold);
    memcpy(p_rsp_tlv->data, &tk_report_threshold, sizeof(tk_report_threshold));
    p_rsp_tlv->len = sizeof(tk_oam_report_threshold_set_t);
    len += sizeof(tk_oam_report_threshold_set_t);

    pOut += sizeof(tk_oam_tlv_t) - 1 + p_rsp_tlv->len;
    p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    p_rsp_tlv->branch = branch;
    p_rsp_tlv->leaf = htons(leaf);
    if(!retCode)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;
    len += sizeof(tk_oam_tlv_t) - 1;

    return len;
}

static oam_uint32
tk_oam_tbl_size_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    oam_uint16  tbl_size = 0;

    tk_oam_mac_limit_get_adapt(mgmtObj.port, &tbl_size);
    tbl_size = htons(tbl_size);
    memcpy(p_rsp_tlv->data, &tbl_size, sizeof(tbl_size));

    OAM_ORG_LOG( "\r\ntk tbl size get\r\n");
    p_rsp_tlv->len = sizeof(oam_uint16);
    len += p_rsp_tlv->len;

    return len;
}

static oam_uint32
tk_oam_tbl_size_set(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    tk_oam_mac_table_size_get_t *p_set =  NULL;
    oam_status retCode = OAM_E_OK;

    if(p_tlv->len != sizeof(tk_oam_mac_table_size_get_t))
    {    OAM_ORG_LOG( "\r\ntk set tbl size:  inv len %d %d\r\n", (oam_uint32)p_tlv->len, (oam_uint32)sizeof(tk_oam_mac_table_size_get_t));}
    else
    {
        p_set = (tk_oam_mac_table_size_get_t *)p_tlv->data;
        p_set->mac_table_size = ntohs(p_set->mac_table_size);

        OAM_ORG_LOG( "\r\nport: %d, mac_table_size: 0x%x\r\n", mgmtObj.port, p_set->mac_table_size);

        retCode = tk_oam_mac_limit_set_adapt(mgmtObj.port, p_set->mac_table_size);

        if(!retCode)
            p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
        else
            p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;
    }

    return len;

}

static oam_uint32
tk_oam_onu_age_time_set(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    tk_oam_mac_age_limit_get_t *p_set =  NULL;
    oam_status retCode = OAM_E_OK;

    if(p_tlv->len != sizeof(tk_oam_mac_age_limit_get_t))
    {    OAM_ORG_LOG( "\r\nstk set onu age_limit: inv len %d %d\r\n", (oam_uint32)p_tlv->len, (oam_uint32)sizeof(tk_oam_mac_age_limit_get_t));}
    else
    {
        p_set = (tk_oam_mac_age_limit_get_t *)p_tlv->data;
        p_set->mac_age_limit = ntohs(p_set->mac_age_limit);

        OAM_ORG_LOG( "\r\ntk set onu age_limit: mac_age_limit 0x%x\r\n", p_set->mac_age_limit);

        retCode = tk_oam_fdb_age_time_set_adapt(p_set->mac_age_limit);
        if(!retCode)
            p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
        else
            p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;
    }

    return len;
}

static oam_uint32
tk_oam_onu_age_time_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    oam_uint16 age_limit;

    tk_oam_fdb_age_time_get_adapt(&age_limit);
    age_limit = htons(age_limit);
    memcpy(p_rsp_tlv->data, &age_limit, sizeof(age_limit));

    OAM_ORG_LOG( "\r\ntk age limit get: %d\r\n", age_limit);
    p_rsp_tlv->len = sizeof(age_limit);
    len += p_rsp_tlv->len;

    return len;
}

static oam_uint32
tk_oam_onu_lrn_mode_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint8 lrn_mode = 0;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;

    tk_oam_lrn_mode_get_adapt(mgmtObj.port, &lrn_mode);
    //lrn_mode = 1;
    OAM_ORG_LOG( "\r\nlrn mode:%d\r\n", lrn_mode);
    memcpy(p_rsp_tlv->data, &lrn_mode, sizeof(lrn_mode));
    p_rsp_tlv->len = sizeof(oam_uint8);
    len += p_rsp_tlv->len;

    return len;
}

static oam_uint32
tk_oam_onu_lrn_mode_set(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint8 lrn_mode;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;

    memcpy(&lrn_mode, p_tlv->data, sizeof(oam_uint8));

    OAM_ORG_LOG( "\r\ntk set onu lrn_mode: lrn_mode 0x%x\r\n", lrn_mode);

    //put termination TLV
    if(OAM_E_OK == tk_oam_lrn_mode_set_adapt(mgmtObj.port, lrn_mode))
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

static oam_uint32
tk_oam_enc_key_expires_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    //oam_uint16 enc_expires = 0;
    //oam_uint32 aal_rc =0;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;

    //aal_rc = aal_tk_oam_onu_enc_key_expire_get(&enc_expires);
    //enc_expires = htons(enc_expires);
    //memcpy(p_rsp_tlv->data, &enc_expires, sizeof(oam_uint16));
    OAM_ORG_LOG( "\r\ntk enc key expires get\r\n");
    p_rsp_tlv->len = sizeof(oam_uint16);
    len += p_rsp_tlv->len;

    return len;
}

static oam_uint32
tk_oam_enc_key_expires_set(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint16 enc_expires = 0;
    oam_uint32 aal_rc =0;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;

    memcpy(&enc_expires, p_tlv->data, sizeof(oam_uint16));
    enc_expires = ntohs(enc_expires);

    OAM_ORG_LOG( "\r\ntk set enc_key_expires: enc_expires 0x%x\r\n", enc_expires);

    //put termination TLV

    if(!aal_rc)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

static oam_uint32
tk_oam_onu_bridge_stp_set(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint8 mode;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    mode = *(p_tlv->data);
    OAM_ORG_LOG( "mode 0x%x\r\n", mode);

#ifdef HAVE_MPORTS
    if(mode == 0 || mode == 3){
        retCode = tk_oam_onu_loop_detect_set_adapt(1);
        retCode = tk_oam_onu_stp_set_adapt(0);
        }
    else if(mode == 1){
        /*BPDU will pass*/
        retCode = tk_oam_onu_stp_enable_set_adapt(0);
        }
    else if(mode == 2){
        retCode = tk_oam_onu_loop_detect_set_adapt(0);
        retCode = tk_oam_onu_stp_set_adapt(1);
        }
#else
    if(mode == 0)
        retCode = tk_oam_onu_loop_detect_set_adapt(0);
    else if(mode == 2 || mode == 3)
        retCode = tk_oam_onu_loop_detect_set_adapt(1);
#endif

    if(retCode == OAM_E_OK)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

static oam_uint32
tk_oam_onu_bridge_stp_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint8 mode = 0;
    oam_uint8 action = 0;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;

    tk_oam_onu_loop_detect_get_adapt(&mode);
    if(mode == 1)
        mode = 3;
    else{
#ifdef HAVE_MPORTS
        /*Get BPDU action*/
        tk_oam_onu_bpdu2cpu_get_adapt(&action);
#endif

        if(action == 0)
            mode = 1;
        else{
        tk_oam_onu_stp_get_adapt(&mode);
        if(mode == 1)
            mode = 2;
        else
            mode = 0;
        }
    }

    OAM_ORG_LOG( "mode 0x%x\r\n", mode);

    memcpy(p_rsp_tlv->data, &mode, sizeof(mode));
    p_rsp_tlv->len = sizeof(oam_uint8);
    len += p_rsp_tlv->len;

    return len;
}

static oam_uint32
tk_oam_uni_phy_enable(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint8 enable;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    enable = *(p_tlv->data);
    OAM_ORG_LOG( "\r\nonu mac enable 0x%x\r\n", enable);

    if(mgmtObj.port > OAM_UNI_PORT_NUM_MAX){
        return len;
    }

    if(g_uni_enable[mgmtObj.port-1] != enable)
    {
        retCode = tk_oam_uni_phy_admin_adapt(mgmtObj.port,enable);
        g_uni_enable[mgmtObj.port-1] = enable;
    }

    if(retCode == OAM_E_OK)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

static oam_uint32
tk_oam_autoneg_admin_set(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint8 auto_admin = 2, enable = 1;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    auto_admin = *(p_tlv->data);
    OAM_ORG_LOG( "\r\ntk set autoneg_admin: auto_admin 0x%x\r\n", auto_admin);
    if(auto_admin == 2){
        enable = 1;
    }else if(auto_admin == 1){
        enable = 0;
    }

    if (mgmtObj.port > OAM_UNI_PORT_NUM_MAX){
        return len;
    }

    if (enable != g_auto_nego[mgmtObj.port-1])
    {
        retCode = tk_oam_port_auto_admin_set_adapt(mgmtObj.port,enable);
        g_auto_nego[mgmtObj.port-1] = enable;
    }

    if(retCode == OAM_E_OK)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

/* branch/leaf: 07 00 53 */
static oam_uint32
tk_oam_port_auto_neg_tech_adv_set(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint16 i = 0;
    oam_uint16 param = 0;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    oam_uint16 count = p_tlv->len/2;
    oam_status retCode = OAM_E_OK;

    OAM_ORG_LOG( "\r\ntk set onu auto neg tech adv: len = %d\r\n", p_tlv->len);

    if (mgmtObj.port > OAM_UNI_PORT_NUM_MAX){
        return len;
    }

    /* when set auto-neg on, p_tlv->len always be 0x80 */
    if(p_tlv->len == TK_VAR_RSP_CODE_OK)
    {
        if (g_flow_control[mgmtObj.port-1] != 0)
        {
            retCode = tk_oam_port_flow_ctrl_enable_adapt(mgmtObj.port,0);
        }
    }
    else
    {
        /* if len/value is 04 xx xx 01 38, it means flow control enable */
        if(p_tlv->len <= 10) {
                for(i=0; i<count; i++)
                {
                    memcpy(&param, p_tlv->data+i*2, sizeof(oam_uint16));
                    retCode = tk_oam_port_auto_neg_tech_adv_set_adapt(mgmtObj.port, ntohs(param));
                }
        }
    }

    if(retCode == OAM_E_OK)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

static oam_uint32
tk_oam_port_auto_neg_tech_adv_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint16 rsp_value = 0;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    oam_status retCode = OAM_E_OK;

    OAM_ORG_LOG( "\r\ntk onu auto neg tech adv get\r\n");
    p_rsp_tlv->len = sizeof(oam_uint16);
    retCode = tk_oam_port_auto_neg_tech_adv_get_adapt(mgmtObj.port, &rsp_value);

    rsp_value = htons(rsp_value);
    memcpy(p_rsp_tlv->data, &rsp_value, sizeof(oam_uint16));
    len += p_rsp_tlv->len;

    pOut += sizeof(tk_oam_tlv_t) - 1 + p_rsp_tlv->len;
    p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    p_rsp_tlv->branch = branch;
    p_rsp_tlv->leaf = htons(leaf);
    if(retCode == OAM_E_OK)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    len += sizeof(tk_oam_tlv_t) - 1;
    return len;
}

static oam_uint32
tk_oam_port_status_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint8 tmp_value = 0;
    oam_uint16 mac_ctrl = 0;
    tk_oam_port_status_t port_status;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;

    OAM_ORG_LOG( "\r\ntk port status get\r\n");

    p_rsp_tlv->len = sizeof(oam_uint8);

    tk_oam_port_status_get_adapt(mgmtObj.port, &port_status);

    if(branch == TK_OAM_BRANCH_ATTRIB)
    {
        switch(leaf)
        {
            case TK_OAM_ATTRIB_LEAF_LEN_ERR_DISCARD:
                tmp_value = 0;
                break;
            case TK_OAM_ATTRIB_LEAF_ONU_MAC_ST_ENABLE:
                memcpy(&tmp_value, &port_status.mac_admin, sizeof(port_status.mac_admin));
                break;
            case TK_OAM_ATTRIB_LEAF_MAU_MED_AVAIL:
                memcpy(&tmp_value, &port_status.link, sizeof(port_status.link));
                break;
            case TK_OAM_ATTRIB_LEAF_PHY_TYPE:
                memcpy(&tmp_value, &port_status.phy_type, sizeof(port_status.phy_type));
                break;
            case TK_OAM_ATTRIB_LEAF_MAC_CTRL_SUPP:
                #if 0
                memcpy(&mac_ctrl, &port_status.mac_ctrl, sizeof(port_status.mac_ctrl));
                p_rsp_tlv->len = sizeof(oam_uint16);
                mac_ctrl = htons(mac_ctrl);
                memcpy(p_rsp_tlv->data, &mac_ctrl, p_rsp_tlv->len);
                len += p_rsp_tlv->len;
                #endif
                if (mgmtObj.port > OAM_UNI_PORT_NUM_MAX){
                    return len;
                }
                if(g_flow_control[mgmtObj.port-1]) /* flow control enable, return 07 00 5d 02 01 38 */
                {
                    p_rsp_tlv->len = 2;
                    p_rsp_tlv->data[0] = 0x01;
                    p_rsp_tlv->data[1] = 0x38;
                    len += p_rsp_tlv->len;
                }
                else /* flow control disable, return 07 00 5d 80 */
                {
                    p_rsp_tlv->len = 0x80;
                }
                return len;
                break;
            case TK_OAM_ATTRIB_LEAF_DUPLEX_STATUS:
                memcpy(&tmp_value, &port_status.duplex, sizeof(port_status.duplex));
                break;
            case TK_OAM_ATTRIB_LEAF_AUTONEG_ADMIN:
                memcpy(&tmp_value, &port_status.auto_neg, sizeof(port_status.auto_neg));
                break;
            default:
                p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;
                return len;
                break;

        }

    }

    memcpy(p_rsp_tlv->data, &tmp_value, p_rsp_tlv->len);
    len += p_rsp_tlv->len;

    return len;
}

static oam_uint32
tk_oam_onu_phy_err_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;

    OAM_ORG_LOG( "\r\ntk phy error get\r\n");

    p_rsp_tlv->branch = branch;
    p_rsp_tlv->leaf =htons( leaf);
    p_rsp_tlv->len = 0xa1;


    return len;
}


static oam_uint32
tk_oam_onu_f9_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;

    OAM_ORG_LOG( "\r\ntk f9 error get\r\n");

    p_rsp_tlv->branch = branch;
    p_rsp_tlv->leaf =htons( leaf);
    p_rsp_tlv->len = 0xa1;


    return len;
}

static oam_uint32 tk_oam_port_us_policer_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    oam_status retCode = OAM_E_OK;
    epon_tk_us_policer_traffic_config_t rsp_value;

    memset(&rsp_value, 0, sizeof(epon_tk_us_policer_traffic_config_t));
    retCode = tk_oam_onu_us_policer_get_adapt(mgmtObj.port, &rsp_value);

    rsp_value.config.rate = htonl(rsp_value.config.rate);
    rsp_value.num_levels = 1;

    OAM_ORG_LOG( "\r\nrate %x, traffic_type %x\r\n", rsp_value.config.rate, rsp_value.config.traffic_type);

    p_rsp_tlv->len = sizeof(epon_tk_us_policer_traffic_config_t);
    memcpy(p_rsp_tlv->data, &rsp_value, sizeof(epon_tk_us_policer_traffic_config_t));
    len += sizeof(epon_tk_us_policer_traffic_config_t);

    pOut += sizeof(tk_oam_tlv_t) - 1 + sizeof(epon_tk_us_policer_traffic_config_t);
    p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    p_rsp_tlv->branch = branch;
    p_rsp_tlv->leaf = htons(leaf);
    if(retCode == OAM_E_OK)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    len += sizeof(tk_oam_tlv_t) - 1;

    return len;
}


static oam_uint32 tk_oam_port_us_policer_set(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint8 i =0, num_level = 0;
    epon_tk_us_policer_traffic_config_t *p_set = NULL;
    epon_tk_us_policer_config_t *p_us_pol = NULL;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    p_set = (epon_tk_us_policer_traffic_config_t *)p_tlv->data;

    OAM_ORG_LOG( "\r\nrate_units 0x%x num_levels 0x%x\r\n", p_set->rate_units, p_set->num_levels);

    OAM_ORG_LOG( "\r\ntraffic_type 0x%x rate 0x%x\r\n", p_set->config.traffic_type, p_set->config.rate);

    num_level = p_set->num_levels;
    num_level = 1;

    for (i = 0; i < num_level; i++) {
        p_us_pol =(epon_tk_us_policer_config_t *) (((char *)&p_set->config) + (i * sizeof(epon_tk_us_policer_config_t)));

        p_us_pol->rate = ntohl(p_us_pol->rate);
        retCode += tk_oam_onu_us_policer_set_adapt(mgmtObj.port, p_set->rate_units, p_us_pol);
    }

    if(retCode == OAM_E_OK)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

static oam_uint32 tk_oam_onu_ds_shaping_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    oam_status retCode = OAM_E_OK;
    epon_tk_ds_shapping_traffic_config_t rsp_value;

    memset(&rsp_value, 0, sizeof(epon_tk_ds_shapping_traffic_config_t));
    retCode = tk_oam_onu_ds_shaping_get_adapt(mgmtObj.port, &rsp_value);

    rsp_value.config.rate = htonl(rsp_value.config.rate);
    rsp_value.config.burst = htons(rsp_value.config.burst);
    rsp_value.config.schedul_algorithm = TK_SCHEDULER_SP;
    rsp_value.num_shapers = 1;

    OAM_ORG_LOG( "\r\nrate %x, burst %x\r\n", rsp_value.config.rate, rsp_value.config.burst);

    p_rsp_tlv->len = sizeof(epon_tk_ds_shapping_traffic_config_t);
    memcpy(p_rsp_tlv->data, &rsp_value, sizeof(epon_tk_ds_shapping_traffic_config_t));
    len += sizeof(epon_tk_ds_shapping_traffic_config_t);

    pOut += sizeof(tk_oam_tlv_t) - 1 + sizeof(epon_tk_ds_shapping_traffic_config_t);
    p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    p_rsp_tlv->branch = branch;
    p_rsp_tlv->leaf = htons(leaf);
    if(retCode == OAM_E_OK)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    len += sizeof(tk_oam_tlv_t) - 1;

    return len;
}

static oam_uint32 tk_oam_onu_ds_shaping_set(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint8 i =0, num_shaper = 0;
    epon_tk_ds_shapping_traffic_config_t *p_set = NULL;
    epon_tk_ds_shapping_config_t *p_ds_shap = NULL;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    oam_status retCode = OAM_E_OK;

    p_set = (epon_tk_ds_shapping_traffic_config_t *)p_tlv->data;

    OAM_ORG_LOG("port %d, traffic_type 0x%x rate_units 0x%x num_shapers 0x%x\r\n",
            mgmtObj.port, p_set->traffic_type, p_set->rate_units, p_set->num_shapers);

    OAM_ORG_LOG( "\r\nqueue_bitmap 0x%x rate 0x%x burst 0x%x schedul_algorithm 0x%x num_weights 0x%x\r\n",
            p_set->config.queue_bitmap, p_set->config.rate, p_set->config.burst, p_set->config.schedul_algorithm, p_set->config.num_weights);

    OAM_ORG_LOG( "\r\nweight 0x%x\r\n", p_set->config.weight);

    num_shaper = p_set->num_shapers;
    num_shaper = 1;

    for (i = 0; i < num_shaper; i++) {
        p_ds_shap =(epon_tk_ds_shapping_config_t *) (((char *)&p_set->config) + (i * sizeof(epon_tk_ds_shapping_config_t)));

        p_ds_shap->rate = ntohl(p_ds_shap->rate);
        p_ds_shap->burst = ntohs(p_ds_shap->burst);

        retCode += tk_oam_onu_ds_shaping_set_adapt(mgmtObj.port, p_set->traffic_type,
        p_set->rate_units, p_ds_shap);
    }

    if(retCode == OAM_E_OK)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

static oam_uint32
tk_oam_port_mac_limit_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint16 mac_limit;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;

    p_rsp_tlv->len = sizeof(mac_limit);
    OAM_ORG_LOG("tk port min mac get\r\n");
    tk_oam_mac_limit_00d1_get_adapt(mgmtObj.port,&mac_limit);
    mac_limit = htons(mac_limit);
    memcpy(p_rsp_tlv->data, &mac_limit, sizeof(mac_limit));

    len += p_rsp_tlv->len;
    return len;
}

static oam_uint32
tk_oam_port_mac_limit_set(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    tk_oam_onu_port_min_mac_limit_set_t *p_set = (tk_oam_onu_port_min_mac_limit_set_t *)p_tlv->data;
    oam_status retCode = OAM_E_OK;
    oam_uint8 port = 0;

    p_set->min_mac_limit = ntohs(p_set->min_mac_limit);
    OAM_ORG_LOG("tk set port min mac: port 0x%x min_mac_limit 0x%x\r\n", mgmtObj.port, p_set->min_mac_limit);
    retCode = tk_oam_mac_limit_00d1_set_adapt(mgmtObj.port,p_set->min_mac_limit);

    if(retCode == OAM_E_OK)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

static oam_uint32
tk_oam_onu_fw_version_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_onu_fw_version_get_rsp_t rsp_value;

    OAM_ORG_LOG( "\r\ntk onu fw version get\r\n");

    p_rsp_tlv->len = sizeof(tk_oam_onu_fw_version_get_rsp_t);
    tk_oam_onu_fw_version_get_adapt(&rsp_value);
    rsp_value.fw_version = htons(rsp_value.fw_version);

    memcpy(p_rsp_tlv->data, &rsp_value, sizeof(tk_oam_onu_fw_version_get_rsp_t));
    len += p_rsp_tlv->len;

    return len;
}

/*This OAM message gets the ONU Extended ID.*/
static oam_uint32
tk_oam_onu_model_name_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_onu_model_name_get_rsp_t rsp_value;

    OAM_ORG_LOG( "\r\ntk onu model name get\r\n");

    p_rsp_tlv->len = sizeof(tk_oam_onu_model_name_get_rsp_t);
    tk_oam_onu_model_name_get_adapt(&rsp_value);
    memcpy(p_rsp_tlv->data, &rsp_value, sizeof(tk_oam_onu_model_name_get_rsp_t));

    len += p_rsp_tlv->len;

    return len;
}

static oam_uint32
tk_oam_onu_port_costranslate_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    oam_uint8 temp_buffer[] = {0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01, 0x02,
        0x02, 0x03, 0x03, 0x04, 0x04, 0x05, 0x05, 0x06, 0x06, 0x07, 0x07};

    OAM_ORG_LOG( "\r\nTK get onu port costranslate\r\n");
    p_rsp_tlv->len = 0x13;

    memcpy(p_rsp_tlv->data, temp_buffer, p_rsp_tlv->len);
    len += p_rsp_tlv->len;

    return len;
}

static oam_uint32
tk_oam_onu_port_prienqueue_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    oam_uint8 temp_buffer[] = {0x00, 0x00, 0x01, 0x00, 0x08, 0x00, 0x00, 0x01,0x00, 0x02, 0x01, 0x03, 0x01, 0x04, 0x02,
        0x05, 0x02, 0x06, 0x03, 0x07, 0x03, 0x00, 0x00, 0x05, 0xdc, 0x06, 0x00, 0x00};

    OAM_ORG_LOG( "\r\nTK get onu port prienqueue\r\n");
    p_rsp_tlv->len = 0x15;

    memcpy(p_rsp_tlv->data, temp_buffer, p_rsp_tlv->len);
    len += p_rsp_tlv->len;

    return len;
}

static oam_uint32
tk_oam_onu_port_vlan_policy_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    epon_tk_port_vlan_policy_config_t rsp_value;
    //oam_uint16 vlan_count;
    //oam_uint16 pvid;

    OAM_ORG_LOG( "\r\ntk onu port vlan policy get\r\n");

    memset(&rsp_value, 0, sizeof(rsp_value));

    p_rsp_tlv->len = sizeof(epon_tk_port_vlan_policy_config_t);

    tk_oam_port_vlan_policy_get_adapt(mgmtObj.port, &rsp_value);
    rsp_value.reserved = htons(rsp_value.reserved);
    rsp_value.vlan = htons(rsp_value.vlan);

    memcpy(p_rsp_tlv->data, &rsp_value, sizeof(epon_tk_port_vlan_policy_config_t));
    len += sizeof(epon_tk_port_vlan_policy_config_t);

    return len;
}


static oam_uint32
tk_oam_onu_port_vlan_policy_set(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    oam_status retCode = OAM_E_OK;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    epon_tk_port_vlan_policy_config_t *p_set = NULL;

    if (mgmtObj.port > OAM_UNI_PORT_NUM_MAX){
        return len;
    }

    if(p_tlv->len != sizeof(epon_tk_port_vlan_policy_config_t)) {
        OAM_ORG_LOG( "\r\nTK set onu port vlan policy: inv len %d %d\r\n", (oam_uint32)p_tlv->len, (oam_uint32)sizeof(epon_tk_port_vlan_policy_config_t));
    } else {
        p_set  = (epon_tk_port_vlan_policy_config_t *)p_tlv->data;

        p_set->reserved = ntohs(p_set->reserved);
        p_set->vlan = ntohs(p_set->vlan);

        OAM_ORG_LOG( "\r\ntk set onu port vlan policy: reserved 0x%x ingress_policy 0x%x egress_policy 0x%x vlan 0x%x\r\n",
                p_set->reserved, p_set->ingress_policy,p_set->egress_policy ,p_set->vlan);

        retCode = tk_oam_port_vlan_policy_set_adapt(mgmtObj.port, p_set);

        if(!retCode)
            p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
        else
            p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;
    }

    return len;
}

static oam_uint32
tk_oam_onu_port_vlan_member_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    epon_tk_port_vlan_member_config_t rsp_value[OAM_TK_PORT_VLAN_NUM_MAX];
    oam_boolean first = FALSE;
    oam_uint32 idx;
    oam_uint16 vlan_count = 0;
    oam_uint16 tmp_count = htons(1);
    epon_tk_port_vlan_member_config_t *buffoffset;

    memset(rsp_value, 0, sizeof(rsp_value));

    tk_oam_port_vlan_member_get_adapt(mgmtObj.port, &vlan_count, rsp_value);

    if(!vlan_count) {
            p_rsp_tlv->len = 2;
            len += p_rsp_tlv->len;
    }
    else {
        for (idx = 0; idx < vlan_count; idx++) {
            p_rsp_tlv->len = 9;

            if(!first)
                {
                    first = TRUE;
                    len += p_rsp_tlv->len;
                }
            else
                {
                    pOut += sizeof(tk_oam_tlv_t) - 1 + p_rsp_tlv->len;
                    p_rsp_tlv = (tk_oam_tlv_t *)pOut;
                    p_rsp_tlv->branch = branch;
                    p_rsp_tlv->leaf = htons(leaf);
                    p_rsp_tlv->len = 9;
                    len += sizeof(tk_oam_tlv_t) - 1 + p_rsp_tlv->len;
                }

            memcpy(p_rsp_tlv->data, &tmp_count, sizeof(oam_uint16));
            rsp_value[idx].ingress_vlan = htons(rsp_value[idx].ingress_vlan);
            rsp_value[idx].egress_vlan = htons(rsp_value[idx].egress_vlan);
            buffoffset = (epon_tk_port_vlan_member_config_t *)(p_rsp_tlv->data + sizeof (oam_uint16));
            memcpy(buffoffset, &rsp_value[idx], sizeof(epon_tk_port_vlan_member_config_t));
        }
    }

    return len;
}

static oam_uint32
tk_oam_onu_port_vlan_member_set(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint16 idx = 0;
    oam_status retCode = OAM_E_OK;


    oam_uint8 old_ktIndex;
    //ca_callback_context_t context;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    tk_oam_onu_port_vlan_member_set_t2 *p_set = NULL;
    epon_tk_port_vlan_member_config_t *tmp_vlan_mbr = NULL;
    epon_tk_port_vlan_member_config_t  vlan_mbr;


    p_set  = (tk_oam_onu_port_vlan_member_set_t2 *)p_tlv->data;
    p_set->vlan_count = ntohs(p_set->vlan_count);

    OAM_ORG_LOG( "\r\ntk set onu port vlan member: vlan_count 0x%x\r\n", p_set->vlan_count);

    if (mgmtObj.port > OAM_UNI_PORT_NUM_MAX){
        return len;
    }

    old_ktIndex = mgmtObj.port;
    if(mgmtObj.port == 0)
        mgmtObj.port = 0x10;
    else
        mgmtObj.port = mgmtObj.port-1;

    if(p_tlv->len == 2 && p_set->vlan_count == 0) {

        if(!retCode)
            p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
        else
            p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;
    }
    else if(p_tlv->len != sizeof(tk_oam_onu_port_vlan_member_set_t2)) {
        OAM_ORG_LOG( "\r\nTK_OAM_ATTRIB_LEAF_ONU_PORT_VLAN_MEMBER inv len %d %d\r\n", (oam_uint32)p_tlv->len, (oam_uint32)sizeof(tk_oam_onu_port_vlan_member_set_t2));
    }
    else {

        tmp_vlan_mbr = (epon_tk_port_vlan_member_config_t *) &p_set->config;

        for (idx = 0; idx < p_set->vlan_count; idx++) {
            tmp_vlan_mbr = (epon_tk_port_vlan_member_config_t *)
                (((char *)&p_set->config) +
                 (sizeof (epon_tk_port_vlan_member_config_t) * idx));

            tmp_vlan_mbr->ingress_vlan = ntohs(tmp_vlan_mbr->ingress_vlan);
            tmp_vlan_mbr->egress_vlan = ntohs(tmp_vlan_mbr->egress_vlan);
            retCode = tk_oam_port_vlan_member_set_adapt(old_ktIndex, tmp_vlan_mbr);
        }

        if(!retCode)
            p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
        else
            p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;
    }

    return len;
}

static oam_uint32
tk_oam_onu_filter_table_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;

    OAM_ORG_LOG( "\r\ntk onu filter table get\r\n");
#if KT_OAM_DEBUG
    tk_oam_onu_filter_table_get_rsp_t rsp_value;
    oam_uint32 clen =0;

    OAM_ORG_LOG( "\r\nTK_OAM_ATTRIB_LEAF_ONU_UP/DOWN_FILTER_TABLE get\r\n");

    //aal_rc = aal_tk_oam_onu_filter_table_get(oam_context.tk_iop_port_id, ntohs(p_tlv->leaf), &rsp_value);

    clen = sizeof(epon_tk_filter_table_clauses_t) *(ONU_MAX_FILTER - rsp_value.num_clauses);
    memcpy(p_rsp_tlv->data, &rsp_value, p_rsp_tlv->len);
    len += sizeof(tk_oam_tlv_t) - 1 + p_rsp_tlv->len;
#endif

    p_rsp_tlv->len = sizeof(tk_oam_onu_filter_table_get_rsp_t);
    len += p_rsp_tlv->len;

    return len;
}

static oam_uint32
tk_oam_add_new_rule(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint8 i =0;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    epon_tk_rule_t *p_set = (epon_tk_rule_t *)p_tlv->data;
    oam_status retCode = OAM_E_OK;

    OAM_ORG_LOG( "\r\ntk add new rule: direction 0x%x action_para[0] 0x%x action_para[1] 0x%x action_para[2] 0x%x action_para[3] 0x%x action_para[4] 0x%x action_para[5] 0x%x\r\n",
            p_set->direction, p_set->action_parameter[0],
            p_set->action_parameter[1], p_set->action_parameter[2],
            p_set->action_parameter[3], p_set->action_parameter[4],
            p_set->action_parameter[5]);

    OAM_ORG_LOG( "\r\nrule_pre 0x%x rule_action 0x%x num_rule 0x%x\r\n",
            p_set->rule_pre, p_set->rule_action, p_set->num_rule);

    for( ; i< p_set->num_rule; i++)
    {
        OAM_ORG_LOG( "\r\nfield 0x%x match_value[0] 0x%x match_value[1] 0x%x match_value[2] 0x%x match_value[3] 0x%x match_value[4] 0x%x match_value[5] 0x%x match_value[6] 0x%x match_value[7] 0x%x operator 0x%x\r\n",
                p_set->cond[i].field, p_set->cond[i].match_value[0],
                p_set->cond[i].match_value[1], p_set->cond[i].match_value[2],
                p_set->cond[i].match_value[3], p_set->cond[i].match_value[4],
                p_set->cond[i].match_value[5], p_set->cond[i].match_value[6],
                p_set->cond[i].match_value[7], p_set->cond[i].operator);
    }

    retCode = tk_oam_add_new_rule_adapt(mgmtObj.port,p_set);

    if(!retCode)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    pOut += sizeof(tk_oam_tlv_t) - 1;
    p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    p_rsp_tlv->branch = p_tlv->branch;
    p_rsp_tlv->leaf = p_tlv->leaf;
    if(!retCode)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    len += sizeof(tk_oam_tlv_t) - 1;

    return len;
}

static oam_uint32
tk_oam_del_new_rule(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint8 i =0;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    epon_tk_rule_t *p_set = (epon_tk_rule_t *)p_tlv->data;
    oam_status retCode = OAM_E_OK;

    OAM_ORG_LOG( "\r\ntk del new rule: direction 0x%x action_para[0] 0x%x action_para[1] 0x%x action_para[2] 0x%x action_para[3] 0x%x action_para[4] 0x%x action_para[5] 0x%x\r\n",
            p_set->direction, p_set->action_parameter[0],
            p_set->action_parameter[1], p_set->action_parameter[2],
            p_set->action_parameter[3], p_set->action_parameter[4],
            p_set->action_parameter[5]);

    OAM_ORG_LOG( "\r\nrule_pre 0x%x rule_action 0x%x num_rule 0x%x\r\n",
            p_set->rule_pre, p_set->rule_action, p_set->num_rule);

    for( ; i< p_set->num_rule; i++)
    {
        OAM_ORG_LOG( "\r\nfield 0x%x match_value[0] 0x%x match_value[1] 0x%x match_value[2] 0x%x match_value[3] 0x%x match_value[4] 0x%x match_value[5] 0x%x match_value[6] 0x%x match_value[7] 0x%x operator 0x%x\r\n",
                p_set->cond[i].field, p_set->cond[i].match_value[0],
                p_set->cond[i].match_value[1], p_set->cond[i].match_value[2],
                p_set->cond[i].match_value[3], p_set->cond[i].match_value[4],
                p_set->cond[i].match_value[5], p_set->cond[i].match_value[6],
                p_set->cond[i].match_value[7], p_set->cond[i].operator);
    }

    retCode = tk_oam_del_new_rule_adapt(mgmtObj.port,p_set);

    if(!retCode)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

static oam_uint32
tk_oam_del_all_rule(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    oam_status retCode = OAM_E_OK;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;

    OAM_ORG_LOG( "\r\ntk del all rule\r\n");

    retCode = tk_oam_del_all_rule_adapt(mgmtObj.port);

    if(!retCode)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

static oam_uint32
tk_oam_rate_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_rate_t oam_rate;

    OAM_ORG_LOG( "\r\ntk oam rate get\r\n");

    len += p_rsp_tlv->len;
    oam_rate.min = 20;
    oam_rate.max = 10;
    memcpy(p_rsp_tlv->data, &oam_rate, sizeof(tk_oam_rate_t));

    pOut += sizeof(tk_oam_tlv_t) - 1 + p_rsp_tlv->len;
    p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    p_rsp_tlv->branch = branch;
    p_rsp_tlv->leaf = htons(leaf);
    p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    len += sizeof(tk_oam_tlv_t) - 1;

    return len;
}

static oam_uint32
tk_oam_rate_set(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;

    if(p_tlv->len != sizeof(tk_oam_rate_t))
    {    OAM_ORG_LOG( "\r\ntk set oam rate: inv len %d %d\r\n", (oam_uint32)p_tlv->len, (oam_uint32)sizeof(tk_oam_rate_t));}
    else
    {
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    }

    return len;
}

static oam_uint32
tk_oam_queue_config_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;

    OAM_ORG_LOG( "\r\ntk queue config get\r\n");
    //p_rsp_tlv->len = 15; /*hard-coded for now as we don't know its meaning*/
    //aal_rc = aal_tk_oam_onu_queue_config_get(p_rsp_tlv->data, 15);
    //len += sizeof(tk_oam_tlv_t) - 1 + 15;

    p_rsp_tlv->len = 15;
    len += p_rsp_tlv->len;

    return len;
}

static oam_uint32
tk_oam_igmp_grp_get(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    oam_uint32   group_count = 0, i;
    tk_oam_igmp_group_info_t *rsp_value = NULL;
    tk_oam_igmp_group_info_t   temp_group;
    oam_boolean first = FALSE;

    /* this dosent match the TK oam spec.
     * but this works because we captured the tk
     * reply and doing the same as they did
     */

    tk_oam_igmp_grp_get_adapt(&temp_group);

    group_count = temp_group.group_count;

    OAM_ORG_LOG( "\r\ntk igmp grp get count: %d\r\n", group_count);
    for (i = 0; i < group_count; i++) {

        p_rsp_tlv->len = (sizeof(tk_oam_igmp_group_t))  + sizeof(oam_uint8);
        if(!first)
            {
                first = TRUE;
                len += p_rsp_tlv->len;
            }
        else
            {
                pOut += sizeof(tk_oam_tlv_t) - 1 + p_rsp_tlv->len;
                p_rsp_tlv = (tk_oam_tlv_t *)pOut;
                p_rsp_tlv->branch = p_tlv->branch;
                p_rsp_tlv->leaf = htons(p_tlv->leaf);
                p_rsp_tlv->len = (sizeof(tk_oam_igmp_group_t))  + sizeof(oam_uint8);
                len += sizeof(tk_oam_tlv_t) - 1 + p_rsp_tlv->len;
            }

        rsp_value = (tk_oam_igmp_group_info_t *)p_rsp_tlv->data;
        rsp_value->group_count = 1;
        rsp_value->groups[0].group_addr = htonl(temp_group.groups[i].group_addr);
        rsp_value->groups[0].port_bitmap = temp_group.groups[i].port_bitmap;
        rsp_value->groups[0].reserved[0] = 0;
        rsp_value->groups[0].reserved[1] = 0;
    }

    if(group_count != 0){
        pOut += sizeof(tk_oam_tlv_t) - 1 + p_rsp_tlv->len;
        p_rsp_tlv = (tk_oam_tlv_t *)pOut;
        p_rsp_tlv->branch = p_tlv->branch;
        p_rsp_tlv->leaf = htons(p_tlv->leaf);
        rsp_value = (tk_oam_igmp_group_info_t *)p_rsp_tlv->data;
        rsp_value->group_count = 0;
        p_rsp_tlv->len = 1;
        len += sizeof(tk_oam_tlv_t);
    }
    else{
        rsp_value = (tk_oam_igmp_group_info_t *)p_rsp_tlv->data;
        rsp_value->group_count = 0;
        p_rsp_tlv->len = 1;
        len += 1;
    }

    return len;
}

static oam_uint32
tk_oam_igmp_grp_add(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint8 i =0;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    tk_oam_igmp_group_info_t *p_set = (tk_oam_igmp_group_info_t *)p_tlv->data;
    oam_status ret = OAM_E_OK;

    OAM_ORG_LOG( "\r\ntk add igmp grp: group_count 0x%x\r\n", p_set->group_count);
    for(i= 0 ; i< TK_IGMP_MAX_GROUP; i++)
    {
        p_set->groups[i].group_addr = ntohl(p_set->groups[i].group_addr);
        OAM_ORG_LOG( "\r\ngroup_addr 0x%x port_bitmap 0x%x reserved[0] 0x%x reserved[1] 0x%x\r\n",
                p_set->groups[i].group_addr, p_set->groups[i].port_bitmap, p_set->groups[i].reserved[0], p_set->groups[i].reserved[1]);
    }

    ret = tk_oam_igmp_grp_add_adapt(p_set);
    if(!ret)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

static oam_uint32
tk_oam_igmp_grp_del(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    oam_uint8 i =0;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    tk_oam_igmp_group_info_t *p_set = (tk_oam_igmp_group_info_t *)p_tlv->data;
    oam_status ret = OAM_E_OK;

    OAM_ORG_LOG( "\r\ntk del igmp grp: group_count 0x%x\r\n", p_set->group_count);

    for(i = 0 ; i< TK_IGMP_MAX_GROUP; i++)
    {
        p_set->groups[i].group_addr = ntohl(p_set->groups[i].group_addr);

        OAM_ORG_LOG( "\r\ngroup_addr 0x%x port_bitmap 0x%x reserved[0] 0x%x reserved[1] 0x%x\r\n",
                p_set->groups[i].group_addr, p_set->groups[i].port_bitmap, p_set->groups[i].reserved[0], p_set->groups[i].reserved[1]);
    }

    ret = tk_oam_igmp_grp_del_adapt(p_set);
    if(!ret)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

static oam_uint32
tk_oam_igmp_config_get(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_igmp_config_get_t *p_get = (tk_oam_igmp_config_get_t *)p_rsp_tlv->data;
    oam_status ret = OAM_E_OK;

    p_rsp_tlv->len = sizeof(tk_oam_igmp_config_get_t);

    ret = tk_oam_igmp_config_get_adapt(p_get);
    OAM_ORG_LOG( "\r\nrobust_count 0x%x lmq_count 0x%x uni_count 0x%x group_count 0x%x queue_count 0x%x grp_fwd_qa 0x%x snoop_op 0x%x\r\n",
            p_get->robust_count, p_get->lmq_count, p_get->uni_count,
            p_get->ports[0].group_count, p_get->ports[0].queue_count,
            p_get->grp_fwd_qa, p_get->snoop_op);
    if(!ret)
        p_rsp_tlv->len = sizeof(tk_oam_igmp_config_get_t);
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    len += p_rsp_tlv->len;

    return len;
}

static oam_uint32
tk_oam_igmp_config_set(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    tk_oam_igmp_config_get_t *p_set = (tk_oam_igmp_config_get_t *)p_tlv->data;
    oam_status ret = OAM_E_OK;

    p_set->grp_fwd_qa = ntohs(p_set->grp_fwd_qa);

    OAM_ORG_LOG( "\r\nrobust_count 0x%x lmq_count 0x%x uni_count 0x%x group_count 0x%x queue_count 0x%x grp_fwd_qa 0x%x snoop_op 0x%x\r\n",
            p_set->robust_count, p_set->lmq_count, p_set->uni_count,
            p_set->ports[0].group_count, p_set->ports[0].queue_count,
            p_set->grp_fwd_qa, p_set->snoop_op);

    ret = tk_oam_igmp_config_set_adapt(p_set);
    if(!ret)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

static oam_uint32
tk_oam_traffic_enable(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;

    OAM_ORG_LOG( "tk enable traffic\r\n");

    tk_oam_block_data_path_set_adapt(0);
    p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;

    return len;
}

static oam_uint32
tk_oam_traffic_disable(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;

    OAM_ORG_LOG( "tk disable traffic\r\n");

    tk_oam_block_data_path_set_adapt(1);
    p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;

    return len;
}

static oam_uint32
tk_oam_onu_statis_clear(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;

    OAM_ORG_LOG( "\r\ntk clear onu statis\r\n");
    kt_oam_onu_clear_statis_adapt();

    p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;

    return len;
}

static oam_uint32
tk_oam_onu_reset(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;

    OAM_ORG_LOG( "\r\ntk reset onu\r\n");

    kt_oam_action_reset_adapt(1);

    p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;

    return len;
}

static oam_uint32
tk_oam_loopback_enable(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    oam_status retCode = OAM_E_OK;

    OAM_ORG_LOG( "\r\ntk enable loopback\r\n");
    retCode = tk_oam_loopback_enable_adapt(OAM_UNI_PORT_ID1, TRUE);
    oam_build_tk_org_event(TK_ALARM_ID_LOOPBACK, TK_ALARM_RAISE_ENABLE,
             TK_ALARM_UNI_PORT_0, TK_ALARM_CONTEXT_PORT);

    if(retCode == OAM_E_OK)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

static oam_uint32
tk_oam_loopback_disable(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    oam_status retCode = OAM_E_OK;

    OAM_ORG_LOG( "\r\ntk disable loopback\r\n");
    retCode = tk_oam_loopback_enable_adapt(OAM_UNI_PORT_ID1, FALSE);
    oam_build_tk_org_event(TK_ALARM_ID_LOOPBACK, TK_ALARM_RAISE_DISABLE,
             TK_ALARM_UNI_PORT_0, TK_ALARM_CONTEXT_PORT);

    if(retCode == OAM_E_OK)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

static oam_uint32
tk_oam_onu_phy_addr_mdio_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;

    OAM_ORG_LOG( "\r\ntk onu phy addr mdio get\r\n");
#if KT_OAM_DEBUG
    tk_oam_onu_phy_addr_mdio_get_rsp_t rsp_value;
    //tk_oam_onu_phy_addr_mdio_get_t *p_param = NULL;

    p_rsp_tlv->len = sizeof(tk_oam_onu_phy_addr_mdio_get_rsp_t);
    //p_param = (tk_oam_onu_phy_addr_mdio_get_t *)p_tlv->data;

    //aal_rc = aal_tk_oam_onu_phy_addr_mdio_get(p_param, &rsp_value);
    rsp_value.config.reg_address = htons(rsp_value.config.reg_address);
    rsp_value.config.mask = htons(rsp_value.config.mask);
    rsp_value.config.value = htons(rsp_value.config.value);

    memcpy(p_rsp_tlv->data, &rsp_value, sizeof(tk_oam_onu_phy_addr_mdio_get_rsp_t));
    len += sizeof(tk_oam_tlv_t) - 1 + sizeof(tk_oam_onu_phy_addr_mdio_get_rsp_t);
#endif

    p_rsp_tlv->len = sizeof(tk_oam_onu_phy_addr_mdio_get_rsp_t);
    len += p_rsp_tlv->len;


    return len;
}

static oam_uint32
tk_oam_onu_phy_addr_mdio_set(
        oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    tk_oam_onu_phy_addr_mdio_set_t *p_set = NULL;

    p_set = (tk_oam_onu_phy_addr_mdio_set_t *)p_tlv->data;
    p_set->config.reg_address = ntohs(p_set->config.reg_address);
    p_set->config.mask = ntohs(p_set->config.mask);
    p_set->config.value = ntohs(p_set->config.value);

    OAM_ORG_LOG( "\r\ntk set onu phy addr mdio: phy_addr 0x%x reg_addr 0x%x mask 0x%x value 0x%x\r\n",
            p_set->config.phy_address, p_set->config.reg_address, p_set->config.mask, p_set->config.value);

    p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;

    return len;
}

static oam_uint32
tk_oam_gpio_set(oam_uint8 *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8 *pOut)
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t *p_tlv = (tk_oam_tlv_t *)pRecv;
    tk_oam_onu_gpio_set_t *p_set = NULL;
    oam_uint8 i = 0;
    oam_status retCode = OAM_E_OK;

    p_set = (tk_oam_onu_gpio_set_t *)p_tlv->data;
    p_set->port_map = ntohl(p_set->port_map);
    p_set->value_map = ntohl(p_set->value_map);

    for(i=0; i<32; i++)
    {
        if((p_set->port_map>>i) & 0x1)
        {
            OAM_ORG_LOG( "\r\nport = %d value = %d\r\n", i, ((p_set->value_map>>i) & 0x1));
            retCode = tk_oam_gpio_set_adapt(i, ((p_set->value_map>>i) & 0x1));
        }
    }

    if(retCode == OAM_E_OK)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}



static oam_uint32 tk_oam_top_change_time_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_var_cont_val_8_t *p_rsp_tlv = (oam_var_cont_val_32_t *)pOut;

    p_rsp_tlv->hdr.width = sizeof(oam_uint8);
    p_rsp_tlv->value = 0;
    len += sizeof(oam_uint8);

    return len;
}

static oam_uint32 tk_oam_topo_change_cnt_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_var_cont_val_8_t *p_rsp_tlv = (oam_var_cont_val_8_t *)pOut;

    p_rsp_tlv->hdr.width = sizeof(oam_uint8);
    p_rsp_tlv->value = 0;
    len += sizeof(oam_uint8);

    return len;
}


static oam_uint32 tk_oam_designated_root_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_rstp_designated_root_t *root = p_rsp_tlv->data;
    oam_if_t * oam_intf = oam_intf_find_valid();

    p_rsp_tlv->len = sizeof(tk_oam_rstp_designated_root_t);
    root->priority = htons(0x80);
    // source MAC address
    memcpy(root->mac, oam_intf->macaddr, 6);
    len += sizeof(tk_oam_rstp_designated_root_t);

    return len;
}


static oam_uint32 tk_oam_root_path_cost_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_var_cont_val_32_t *p_rsp_tlv = (oam_var_cont_val_32_t *)pOut;

    p_rsp_tlv->hdr.width = sizeof(oam_uint32);
    p_rsp_tlv->value = htonl(0);
    len += sizeof(oam_uint32);

    return len;
}

static oam_uint32 tk_oam_root_port_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_var_cont_val_8_t *p_rsp_tlv = (oam_var_cont_val_8_t *)pOut;

    p_rsp_tlv->hdr.width = sizeof(oam_uint8);
    p_rsp_tlv->value = 1;
    len += sizeof(oam_uint8);

    return len;
}


static oam_uint32 tk_oam_max_age_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_var_cont_val_16_t *p_rsp_tlv = (oam_var_cont_val_16_t *)pOut;

    p_rsp_tlv->hdr.width = sizeof(oam_uint16);
    p_rsp_tlv->value = htons(2000);
    len += sizeof(oam_uint16);

    return len;
}

static oam_uint32 tk_oam_hello_time_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_var_cont_val_16_t *p_rsp_tlv = (oam_var_cont_val_16_t *)pOut;

    p_rsp_tlv->hdr.width = sizeof(oam_uint16);
    p_rsp_tlv->value = htons(200);
    len += sizeof(oam_uint16);

    return len;
}

static oam_uint32 tk_oam_foward_delay_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_var_cont_val_16_t *p_rsp_tlv = (oam_var_cont_val_16_t *)pOut;

    p_rsp_tlv->hdr.width = sizeof(oam_uint16);
    p_rsp_tlv->value = htons(1500);
    len += sizeof(oam_uint16);

    return len;
}


static oam_uint32 tk_oam_rstp_f_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_var_cont_val_16_t *p_rsp_tlv = (oam_var_cont_val_16_t *)pOut;

    p_rsp_tlv->hdr.width = sizeof(oam_uint16);
    p_rsp_tlv->value = htons(0x2c01);
    len += sizeof(oam_uint16);

    return len;
}


static oam_uint32 tk_oam_rstp_10_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_var_cont_val_16_t *p_rsp_tlv = (oam_var_cont_val_16_t *)pOut;

    p_rsp_tlv->hdr.width = sizeof(oam_uint16);
    p_rsp_tlv->value = htons(0x0200);
    len += sizeof(oam_uint16);

    return len;
}

static oam_uint32 tk_oam_rstp_11_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_var_cont_val_16_t *p_rsp_tlv = (oam_var_cont_val_16_t *)pOut;

    p_rsp_tlv->hdr.width = sizeof(oam_uint16);
    p_rsp_tlv->value = htons(0x2c01);
    len += sizeof(oam_uint16);

    return len;
}

static oam_uint32 tk_oam_hold_time_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_var_cont_val_16_t *p_rsp_tlv = (oam_var_cont_val_16_t *)pOut;

    p_rsp_tlv->hdr.width = sizeof(oam_uint16);
    p_rsp_tlv->value = htons(0x0300);
    len += sizeof(oam_uint16);

    return len;
}


static oam_uint32 tk_oam_rstp_13_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_var_cont_val_16_t *p_rsp_tlv = (oam_var_cont_val_16_t *)pOut;

    p_rsp_tlv->hdr.width = sizeof(oam_uint16);
    p_rsp_tlv->value = htons(0x80);
    len += sizeof(oam_uint16);

    return len;
}

static oam_uint32 tk_oam_rstp_port_state_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_tk_rstp_state_t state = 0;
    oam_var_cont_val_8_t *p_rsp_tlv = (oam_var_cont_val_8_t *)pOut;

    p_rsp_tlv->hdr.width = sizeof(oam_uint8);
    // get rstp port state
    oam_tk_rstp_port_state_get_adapt(mgmtObj.port, &state);
    p_rsp_tlv->value = (oam_uint8 *)state;

    len += sizeof(oam_uint8);

    return len;
}


static oam_uint32 tk_oam_rstp_port_23_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_var_cont_val_8_t *p_rsp_tlv = (oam_var_cont_val_8_t *)pOut;

    p_rsp_tlv->hdr.width = sizeof(oam_uint8);
    // get rstp port state
    p_rsp_tlv->value = 0x80;
    len += sizeof(oam_uint8);

    return len;
}


static oam_uint32 tk_oam_rstp_port_24_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_var_cont_val_32_t *p_rsp_tlv = (oam_var_cont_val_32_t *)pOut;

    p_rsp_tlv->hdr.width = sizeof(oam_uint32);
    // get rstp port state
    p_rsp_tlv->value = htonl(0x2710);
    len += sizeof(oam_uint32);

    return len;
}

static oam_uint32 tk_oam_rstp_port_designated_root_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_rstp_designated_root_t *root = p_rsp_tlv->data;
    oam_if_t * oam_intf = oam_intf_find_valid();

    p_rsp_tlv->len = sizeof(tk_oam_rstp_designated_root_t);
    root->priority = htons(0x80);
    // source MAC address
    memcpy(root->mac, oam_intf->macaddr, 6);
    len += sizeof(tk_oam_rstp_designated_root_t);

    return len;
}

static oam_uint32 tk_oam_rstp_port_designated_cost_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_var_cont_val_32_t *p_rsp_tlv = (oam_var_cont_val_32_t *)pOut;
    oam_uint32 *ptr = &p_rsp_tlv->value;
    ptr ++;

    p_rsp_tlv->hdr.width = 2*sizeof(oam_uint32);
    p_rsp_tlv->value = 0;
    *ptr = htonl(0x2710);
    len += 2*sizeof(oam_uint32);

    return len;
}

static oam_uint32 tk_oam_rstp_port_designated_bridge_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_rstp_designated_root_t *root = p_rsp_tlv->data;
    oam_if_t * oam_intf = oam_intf_find_valid();

    p_rsp_tlv->len = sizeof(tk_oam_rstp_designated_root_t);
    root->priority = htons(0x80);
    // source MAC address
    memcpy(root->mac, oam_intf->macaddr, 6);
    len += sizeof(tk_oam_rstp_designated_root_t);

    return len;
}

static oam_uint32 tk_oam_rstp_port_designated_port_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_var_cont_val_16_t *p_rsp_tlv = (oam_var_cont_val_16_t *)pOut;

    p_rsp_tlv->hdr.width = sizeof(oam_uint16);
    p_rsp_tlv->value = mgmtObj.port + 1;
    p_rsp_tlv->value = htons(p_rsp_tlv->value);
    len += sizeof(oam_uint16);

    return len;
}

static oam_uint32 tk_oam_rstp_port_topo_change_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8 branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint32 len = OFFSET_LEN;
    oam_var_cont_val_32_t *p_rsp_tlv = (oam_var_cont_val_32_t *)pOut;
    oam_uint32 *ptr = &p_rsp_tlv->value;
    ptr ++;

    p_rsp_tlv->hdr.width = 2 * sizeof(oam_uint32);
    p_rsp_tlv->value = 0;
    *ptr = 0;
    len += 2 * sizeof(oam_uint32);

    return len;
}

static oam_uint32
tk_oam_queue_stats_get(
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint8  branch,
        oam_uint16 leaf,
        oam_uint8 *pOut )
{
    oam_uint16 port;
    oam_uint16 queue;
    oam_uint32 len = OFFSET_LEN;
    tk_oam_tlv_t *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    oam_uint64 tmp_value = 0;
    oam_status ret = OAM_E_OK;

    static tk_oam_queue_stats_t stats;

    OAM_ORG_LOG( "tk queue stats get, port %d, queue %d\r\n", port, queue);

    tmp_value = 0;

    port = mgmtObj.port;
    queue = mgmtObj.queue;
    
    if(!g_oam_tk_port_stats_get)
    {
        g_oam_tk_port_stats_get = 1;
        ret = tk_oam_queue_stats_get_adapt(port, queue, &stats);
    }
    else
        ret = OAM_E_OK;

    if(OAM_E_OK == ret)
    {
        if(branch == TK_OAM_BRANCH_ATTRIB)
        {
            switch(leaf)
            {
                case TK_OAM_ATTRIB_LEAF_STD_MAC_OCTETS_RX_OK:
                    memcpy(&tmp_value, &stats.rx_octets, sizeof(oam_uint64));
                    break;
                case TK_OAM_ATTRIB_LEAF_STD_MAC_FRAMES_RX_OK:
                    memcpy(&tmp_value, &stats.rx_frames, sizeof(oam_uint64));
                    break;
                case TK_OAM_ATTRIB_LEAF_RX_BYTES_DROPPED:
                    memcpy(&tmp_value, &stats.rx_bytes_drop, sizeof(oam_uint64));
                    break;
                case TK_OAM_ATTRIB_LEAF_RX_FRAMES_DROPPED:
                    memcpy(&tmp_value, &stats.rx_frames_drop, sizeof(oam_uint64));
                    break;
                case TK_OAM_ATTRIB_LEAF_RX_BYTES_DELAYED:
                    memcpy(&tmp_value, &stats.rx_bytes_delay, sizeof(oam_uint64));
                    break;
                case TK_OAM_ATTRIB_LEAF_RX_DELAY:
                    memcpy(&tmp_value, &stats.rx_delay, sizeof(oam_uint64));
                    break;
                case TK_OAM_ATTRIB_LEAF_RX_DELAY_THRESHOLD:
                    memcpy(&tmp_value, &stats.rx_delay_threshold, sizeof(oam_uint64));
                    break;
                
                case TK_OAM_ATTRIB_LEAF_STD_MAC_OCTETS_TX_OK:
                    memcpy(&tmp_value, &stats.tx_octets, sizeof(oam_uint64));
                    break;
                case TK_OAM_ATTRIB_LEAF_STD_MAC_FRAMES_TX_OK:
                    memcpy(&tmp_value, &stats.tx_frames, sizeof(oam_uint64));
                    break;
                case TK_OAM_ATTRIB_LEAF_TX_BYTES_DROPPED:
                    memcpy(&tmp_value, &stats.tx_bytes_drop, sizeof(oam_uint64));
                    break;
                case TK_OAM_ATTRIB_LEAF_TX_FRAMES_DROPPED:
                    memcpy(&tmp_value, &stats.tx_frames_drop, sizeof(oam_uint64));
                    break;
                case TK_OAM_ATTRIB_LEAF_TX_BYTES_DELAYED:
                    memcpy(&tmp_value, &stats.tx_bytes_delay, sizeof(oam_uint64));
                    break;
                case TK_OAM_ATTRIB_LEAF_TX_DELAY:
                    memcpy(&tmp_value, &stats.tx_delay, sizeof(oam_uint64));
                    break;
                case TK_OAM_ATTRIB_LEAF_TX_DELAY_THRESHOLD:
                    memcpy(&tmp_value, &stats.tx_delay_threshold, sizeof(oam_uint64));
                    break;    
                default:
                    p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;
                    OAM_ORG_LOG( "\r\nunknow TK leaf under attrib branch 0x%x\r\n", ntohs(leaf));
                    return len;
                    break;

            }

            tmp_value = htonll(tmp_value);
            p_rsp_tlv->len = sizeof(oam_uint64);
            memcpy(p_rsp_tlv->data, &tmp_value, sizeof(oam_uint64));
            len += p_rsp_tlv->len;
        }

    }
    else{
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;
    }

    return len;
}


/**********************************************************************/
/********* OAM configuration for image upgrade via TFTP ***************/
/**********************************************************************/

oam_uint32 tk_oam_onu_upg_tftp_addr_set(
        oam_uint8  *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8  *pOut)
{
    oam_uint32                 len        = OFFSET_LEN;
    oam_kt_upg_tftp_addr_ctx_t *addr_ctx  = NULL;
    tk_oam_tlv_t               *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t               *p_tlv     = (tk_oam_tlv_t *)pRecv;
    oam_status                 retCode    = OAM_E_OK;

    addr_ctx = (tk_oam_upg_tftp_addr_t *)(p_tlv->data);

    retCode = oam_onu_upg_tftp_addr_set(addr_ctx);

    if(retCode == OAM_E_OK)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

oam_uint32 tk_oam_onu_upg_tftp_auto_reboot_set(
        oam_uint8  *pRecv,
        oam_uint32 *inLen,
        tk_oam_mgmt_obj_t mgmtObj,
        oam_uint32 *outPktLen,
        oam_uint8  *pOut)
{
    oam_uint32                 len        = OFFSET_LEN;
    oam_kt_upg_auto_reboot_e   auto_reboot;
    tk_oam_tlv_t               *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    tk_oam_tlv_t               *p_tlv     = (tk_oam_tlv_t *)pRecv;
    oam_status                 retCode    = OAM_E_OK;

    auto_reboot = *(p_tlv->data);

    retCode = oam_onu_upg_tftp_auto_reboot_set(auto_reboot);

    if(retCode == OAM_E_OK)
        p_rsp_tlv->len = TK_VAR_RSP_CODE_OK;
    else
        p_rsp_tlv->len = TK_VAR_RSP_CODE_UNDETERM;

    return len;
}

oam_uint32 tk_oam_onu_upg_tftp_dl_state_get(
    tk_oam_mgmt_obj_t mgmtObj,
    oam_uint8 branch,
    oam_uint16 leaf,
    oam_uint8 *pOut )
{
    oam_uint32           len        = OFFSET_LEN;
    tk_oam_tlv_t         *p_rsp_tlv = (tk_oam_tlv_t *)pOut;
    oam_uint8            dl_state = 0;
    oam_status           retCode    = OAM_E_OK;

    retCode = oam_onu_upg_tftp_dl_state_get(&dl_state);  
    
    p_rsp_tlv->len     =  sizeof(oam_uint8);
    len                += p_rsp_tlv->len;

    return len;
}
/**********************************************************************/
/********* END OAM configuration for image upgrade via TFTP ***********/
/**********************************************************************/

tk_oam_xlat_rountine_t g_tk_kt_oam_xlat_table[] =
{
#undef xx
#define xx(SEQ,BRANCH,LEAF,LEAF_VAL,GET_FUNC,SET_FUNC)   {BRANCH,LEAF,GET_FUNC,SET_FUNC},
    TK_KT_OAM_BUILD_RESP
};

tk_oam_xlat_rountine_t g_tk_brg_oam_xlat_table[] =
{
#undef xx
#define xx(SEQ,BRANCH,LEAF,LEAF_VAL,GET_FUNC,SET_FUNC)   {BRANCH,LEAF,GET_FUNC,SET_FUNC},
    TK_BRG_OAM_BUILD_RESP
};

tk_oam_xlat_rountine_t g_tk_brg_port_oam_xlat_table[] =
{
#undef xx
#define xx(SEQ,BRANCH,LEAF,LEAF_VAL,GET_FUNC,SET_FUNC)   {BRANCH,LEAF,GET_FUNC,SET_FUNC},
    TK_BRG_PORT_OAM_BUILD_RESP
};

tk_oam_xlat_rountine_t g_tk_queue_oam_xlat_table[] =
{
#undef xx
#define xx(SEQ,BRANCH,LEAF,LEAF_VAL,GET_FUNC,SET_FUNC)   {BRANCH,LEAF,GET_FUNC,SET_FUNC},
    TK_QUEUE_OAM_BUILD_RESP
};

#endif

/******************************************************************************/
/*                           COMMON                                           */
/******************************************************************************/

void oam_tk_send(
        oam_uint32 port,
        oam_llid_t llid,
        oam_uint8 *buf,
        oam_int32 len
        )
{
    oam_int32 result = OAM_E_OK;
    OAM_ORG_LOG("port %d, llid 0x%x\r\n", port, llid);
    
    if(len < OAM_MIN_OAMPDU_SIZE - 4){
        len = OAM_MIN_OAMPDU_SIZE - 4;
    }

    oam_send(llid, OAM_PON_PORT_ID, buf, len);

    if (result != OAM_E_OK) {
        OAM_ORG_LOG("\r\nkt_oam_send: error 0x%x\r\n", result);
    }
}

/******************************************************************************/
/*                           PARSER                                           */
/******************************************************************************/

static oam_status
oam_get_tk_obj_info(
    oam_uint8  *objCont,
    oam_uint32 *objLen,
    tk_oam_mgmt_obj_t *mgmtObj)
{
    tk_oam_var_cont_t *pVarCont = (tk_oam_var_desc_t * )objCont;
    
    oam_tk_link_obj_t *pLinkObj = NULL;
    oam_tk_queue_obj_t *pQueueObj = NULL;
    oam_ext_mode_t mode = OAM_EXT_MODE_DPOE;

    OAM_ASSERT_RET(objCont, OAM_E_ERROR);

    memset(mgmtObj, 0, sizeof(tk_oam_mgmt_obj_t));
    
    if(pVarCont->extBranch == TK_OAM_BRANCH_NAME) {
        mgmtObj->type = ntohs(pVarCont->leaf);
        switch (ntohs(pVarCont->leaf)){
            case TK_OAM_LEAF_PORT_OBJ:      /* the size of port is different for DPOE and KT project */
                {
                    mode = oam_ext_mode_get_adapt();
                    OAM_ORG_LOG("ext mode is %d\r\n", mode);
#ifdef HAVE_DPOE_OAM                     
                    if(mode == OAM_EXT_MODE_DPOE)
                    { 
                        oam_tk_dpoe_port_obj_t *pPortObj = (oam_tk_dpoe_port_obj_t*)objCont;
                        mgmtObj->port = (oam_uint16)(pPortObj->port);
                        *objLen = sizeof(oam_tk_dpoe_port_obj_t);                       
                    }
#endif 
#ifdef HAVE_KT_OAM
                    if(mode == OAM_EXT_MODE_KT)
                    {  
                        oam_tk_port_obj_t *pPortObj = (oam_tk_port_obj_t*)objCont;
                        mgmtObj->port = ntohs(pPortObj->port);
                        *objLen = sizeof(oam_tk_port_obj_t);                        
                    }
#endif                    
                    return OAM_E_OK;
                }
#ifdef HAVE_KT_OAM
            case TK_OAM_LEAF_BRIDGE_OBJ:
            case TK_OAM_LEAF_QUEUE_OBJ:
            case TK_OAM_LEAF_BRIDGE_PORT_OBJ:
                {
                    oam_tk_port_obj_t *pPortObj = (oam_tk_port_obj_t *)objCont;
                    mgmtObj->port = ntohs(pPortObj->port);
                    *objLen = sizeof(oam_tk_port_obj_t);
                    return OAM_E_OK;
                }
            case TK_OAM_LEAF_LPORT_OBJ:
                {
                    pLinkObj = (oam_tk_link_obj_t*)objCont;
                    mgmtObj->port = ntohs(pLinkObj->port);
                    mgmtObj->link = ntohs(pLinkObj->link);
                    *objLen = sizeof(oam_tk_link_obj_t);
                    return OAM_E_OK;
                }
            case TK_OAM_LEAF_PORT_QUEUE_OBJ:
                {
                    pQueueObj = (oam_tk_queue_obj_t*)objCont;
                    mgmtObj->port = ntohs(pQueueObj->port);
                    mgmtObj->queue = ntohs(pQueueObj->queue);
                    *objLen = sizeof(oam_tk_queue_obj_t);
                    return OAM_E_OK;
                }
#endif                
            default:
                return OAM_E_ERROR;
        }
    }

    return OAM_E_ERROR;
}

static oam_uint32
oam_build_tk_obj_instance(
        oam_uint8 *objCont,
        tk_oam_mgmt_obj_t mgmtObj)
{
    tk_oam_var_cont_t *pVarCont = (tk_oam_var_cont_t *)objCont;
    oam_ext_mode_t mode = OAM_EXT_MODE_DPOE;
    
    OAM_ASSERT_RET(objCont, OAM_E_ERROR);

    pVarCont->extBranch = TK_OAM_BRANCH_NAME;
    pVarCont->leaf = htons(mgmtObj.type);

    switch(mgmtObj.type)
    {
        case TK_OAM_LEAF_PORT_OBJ:
        {
            mode = oam_ext_mode_get_adapt();
            OAM_ORG_LOG("ext mode is %d\r\n", mode);
#ifdef HAVE_DPOE_OAM              
            if(mode == OAM_EXT_MODE_DPOE)
            {
                oam_tk_dpoe_port_obj_t *pPortObj = (oam_tk_dpoe_port_obj_t *)objCont; 
                pPortObj->hdr.width = sizeof(oam_tk_dpoe_port_obj_t)-sizeof(tk_oam_var_cont_t);
                pPortObj->port = (oam_uint8)(mgmtObj.port);
                return sizeof(oam_tk_dpoe_port_obj_t);
            }
#endif
#ifdef HAVE_KT_OAM
            if(mode == OAM_EXT_MODE_KT)
            {
                oam_tk_port_obj_t *pPortObj = (oam_tk_port_obj_t *)objCont; 
                pPortObj->hdr.width = sizeof(oam_tk_port_obj_t)-sizeof(tk_oam_var_cont_t);
                pPortObj->port = htons(mgmtObj.port);
                return sizeof(oam_tk_port_obj_t);
            }
#endif  
            return 0;
        }
#ifdef HAVE_KT_OAM        
        case TK_OAM_LEAF_BRIDGE_OBJ:
        case TK_OAM_LEAF_QUEUE_OBJ:
        case TK_OAM_LEAF_BRIDGE_PORT_OBJ:
        {
            oam_tk_port_obj_t *pPortObj = (oam_tk_port_obj_t *)objCont; 
            if(mgmtObj.type == TK_OAM_LEAF_BRIDGE_PORT_OBJ && mgmtObj.port == 0xffff)
            {
                pPortObj->hdr.width = TK_VAR_RSP_CODE_OK;
                return sizeof(oam_var_cont_t);
            }
            pPortObj->hdr.width = sizeof(oam_tk_port_obj_t)-sizeof(tk_oam_var_cont_t);
            pPortObj->port = htons(mgmtObj.port);
            return sizeof(oam_tk_port_obj_t);
        }
        case TK_OAM_LEAF_LPORT_OBJ:
        {
            oam_tk_link_obj_t *pLinkObj = (oam_tk_link_obj_t *)objCont;
            pLinkObj->hdr.width = sizeof(oam_tk_link_obj_t)-sizeof(tk_oam_var_cont_t);
            pLinkObj->port = htons(mgmtObj.port);
            pLinkObj->link = htons(mgmtObj.link);
            return sizeof(oam_tk_link_obj_t);
        }
        case TK_OAM_LEAF_PORT_QUEUE_OBJ:
        {
            oam_tk_queue_obj_t *pQueueObj = (oam_tk_queue_obj_t*)objCont;
            pQueueObj->hdr.width = sizeof(oam_tk_queue_obj_t)-sizeof(tk_oam_var_cont_t);
            pQueueObj->port = htons(mgmtObj.port);
            pQueueObj->queue = htons(mgmtObj.queue);
            return sizeof(oam_tk_queue_obj_t);
        }
#endif
        default:
            return 0;
    }
    
    return 0;
}

tk_oam_get_rountine_t oam_find_tk_get_handler(
    oam_uint16 objType,
    oam_uint8  branch,
    oam_uint16 leaf)
{
    tk_oam_get_rountine_t  get_handler = NULL;
    tk_oam_xlat_rountine_t *xlat_table = NULL;
    oam_uint32 len = 0;
    oam_uint32 i = 0;
    oam_ext_mode_t mode = OAM_EXT_MODE_DPOE;

    mode = oam_ext_mode_get_adapt();

#ifdef HAVE_DPOE_OAM
    if(mode == OAM_EXT_MODE_DPOE)
    {    
        xlat_table = g_tk_dpoe_oam_xlat_table;
        len = sizeof(g_tk_dpoe_oam_xlat_table)/sizeof(tk_oam_xlat_rountine_t);
    }
#endif

#ifdef HAVE_KT_OAM 
    if(mode == OAM_EXT_MODE_KT)
    {  
        if(objType == TK_OAM_LEAF_PORT_QUEUE_OBJ)
        {
            return NULL;
            //xlat_table = g_tk_queue_oam_xlat_table;
            //len = sizeof(g_tk_queue_oam_xlat_table)/sizeof(tk_oam_xlat_rountine_t);
        }
        else if(objType == TK_OAM_LEAF_BRIDGE_OBJ)
        {
            xlat_table = g_tk_brg_oam_xlat_table;
            len = sizeof(g_tk_brg_oam_xlat_table)/sizeof(tk_oam_xlat_rountine_t);
        }
        else if(objType == TK_OAM_LEAF_BRIDGE_PORT_OBJ)
        {
            xlat_table = g_tk_brg_port_oam_xlat_table;
            len = sizeof(g_tk_brg_port_oam_xlat_table)/sizeof(tk_oam_xlat_rountine_t);
        }
        else
        { 
            xlat_table = g_tk_kt_oam_xlat_table;
            len = sizeof(g_tk_kt_oam_xlat_table)/sizeof(tk_oam_xlat_rountine_t);
        }
    }
#endif    
    if(xlat_table)
    {
        for(i =0; i<len; i++)
        {
            if((xlat_table[i].branch == branch)&&(xlat_table[i].leaf == leaf))
            {
                OAM_ORG_LOG("tk oam found get handler: branch 0x%x leaf 0x%04x\r\n", branch, leaf);
                get_handler = xlat_table[i].get_func;
                break;
            }
        }
        if(i == len)
        {
            OAM_ORG_LOG( "tk oam NOT found get handler: branch 0x%x leaf 0x%04x\r\n", branch, leaf);
        }
    }

    return get_handler;
}

tk_oam_set_rountine_t oam_find_tk_set_handler(
    oam_uint16 objType,
    oam_uint8  branch,
    oam_uint16 leaf)
{
    tk_oam_set_rountine_t  set_handler = NULL;
    tk_oam_xlat_rountine_t *xlat_table;
    oam_uint32 len = 0;
    oam_uint32 i = 0;
    oam_ext_mode_t mode = OAM_EXT_MODE_DPOE;

    mode = oam_ext_mode_get_adapt();
    
#ifdef HAVE_DPOE_OAM   
    if(mode == OAM_EXT_MODE_DPOE)
    {       
        xlat_table = g_tk_dpoe_oam_xlat_table;
        len = sizeof(g_tk_dpoe_oam_xlat_table)/sizeof(tk_oam_xlat_rountine_t);
    }
#endif
#ifdef HAVE_KT_OAM 
    if(mode == OAM_EXT_MODE_KT)
    {  
        if(objType == TK_OAM_LEAF_PORT_QUEUE_OBJ)
        {
            return NULL;
            //xlat_table = g_tk_queue_oam_xlat_table;
            //len = sizeof(g_tk_queue_oam_xlat_table)/sizeof(tk_oam_xlat_rountine_t);
        }
        else if(objType == TK_OAM_LEAF_BRIDGE_OBJ)
        {
            xlat_table = g_tk_brg_oam_xlat_table;
            len = sizeof(g_tk_brg_oam_xlat_table)/sizeof(tk_oam_xlat_rountine_t);
        }
        else if(objType == TK_OAM_LEAF_BRIDGE_PORT_OBJ)
        {
            xlat_table = g_tk_brg_port_oam_xlat_table;
            len = sizeof(g_tk_brg_port_oam_xlat_table)/sizeof(tk_oam_xlat_rountine_t);
        }
        else
        { 
            xlat_table = g_tk_kt_oam_xlat_table;
            len = sizeof(g_tk_kt_oam_xlat_table)/sizeof(tk_oam_xlat_rountine_t);
        }
    }
#endif
    if(xlat_table)
    {
        for(i =0 ; i<len; i++)
        {
            if((xlat_table[i].branch == branch)&&(xlat_table[i].leaf == leaf))
            {
                OAM_ORG_LOG( "tk oam found set handler: branch 0x%02x leaf 0x%04x\r\n", branch, leaf);
                set_handler = xlat_table[i].set_func;   
                break;
            }
        }
        if(i == len)
        {
            OAM_ORG_LOG( "tk oam NOT found set handler: branch 0x%x leaf 0x%04x\r\n", branch, leaf);
        }
    }
    return set_handler;
}

static oam_uint32
oam_build_tk_var_cont(
        oam_uint8 *pOutVarCont,
        oam_uint8  branch,
        oam_uint16 leaf,
        oam_uint8  code)
{
    tk_oam_var_cont_t *pVarCont = (tk_oam_var_cont_t *)pOutVarCont;

    pVarCont->extBranch = branch;
    pVarCont->leaf = htons(leaf);
    pVarCont->len = code;

    return sizeof(tk_oam_var_cont_t);

}

static oam_int32 oam_build_tk_var_resp(
        oam_llid_t llid,
        oam_uint8 *frame,
        oam_uint32 lenghth)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE];
    oam_if_t *intf = oam_intf_find(llid);
    tk_oam_pdu_t *pRecvPkt = (tk_oam_pdu_t *)frame;
    tk_oam_pdu_t *pOutPkt = (tk_oam_pdu_t *)pdu;
    tk_oam_var_desc_t *pRecvVarCont = NULL;
    oam_uint8 *pInVarCont = NULL;
    oam_uint8 *pOutVarCont = NULL;
    oam_uint32 totalContentLen = 0;
    oam_uint32 outPktLen = 0;
    tk_oam_mgmt_obj_t mgmtObj;
    oam_uint32 objLen = 0;
    oam_uint32 i = 0, j = 0;
    oam_uint32 length = 0;
    oam_uint8 good_packet = 0;
    tk_oam_get_rountine_t get_handler = NULL;

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);
    if(!frame || lenghth <= sizeof(tk_oam_tlv_t) + sizeof(tk_oam_pdu_t) - 2)
        return 0;

    if (!intf) 
    {
        OAM_ORG_LOG( "interface not found for port 0x%x\r\n", llid);
        return 0;
    }

    /* we need to build header to pass sanity check */
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    memcpy(pOutPkt->oui, pRecvPkt->oui, OAM_OUI_LEN);
    pOutPkt->opcode = TK_OPCODE_GET_RSP;

    outPktLen =  sizeof(tk_oam_pdu_t) - 1;
    pOutVarCont = pOutPkt->data;

    totalContentLen = lenghth - sizeof(tk_oam_pdu_t) + 1;
    pRecvVarCont = (tk_oam_var_desc_t *)pRecvPkt->data;
    pInVarCont = (oam_uint8 *)pRecvVarCont;

    while(totalContentLen>= (sizeof(tk_oam_tlv_t)-1)){
        tk_oam_var_cont_t *pOut = (tk_oam_var_cont_t *)pOutVarCont;

        /*if index-of-instance branch, save index value*/
        switch(pRecvVarCont->extBranch)
        {
            case TK_OAM_BRANCH_NAME:
            {
#ifdef HAVE_KT_OAM            
                g_oam_tk_port_stats_get = 0; /* update counters form ASIC one time for all branch/leaf of one port index */
#endif
                if(OAM_E_OK == oam_get_tk_obj_info((oam_uint8*)pRecvVarCont,&objLen, &mgmtObj))
                {
                    pInVarCont += objLen;
                    pRecvVarCont = (tk_oam_var_desc_t *)pInVarCont;
                    totalContentLen -= objLen;

                    objLen = oam_build_tk_obj_instance(pOutVarCont,mgmtObj);
                    outPktLen += objLen;
                    pOutVarCont += objLen;
                    continue;
                }
                else
                {
                    objLen = oam_build_tk_var_cont(pOutVarCont, TK_OAM_BRANCH_NAME, ntohs(pRecvVarCont->leaf), TK_VAR_RSP_CODE_OK);                
                    outPktLen += objLen;
                    pOutVarCont += objLen;
                    goto send_resp;
                }
                break;
            }
            case OAM_ATTRIB_BRANCH_NULL:
                if(!good_packet)
                    goto ignored;/*clause57 57.6.3 a)*/
                else
                    goto send_resp;
                break;
            case TK_OAM_BRANCH_ATTRIB:
            case TK_OAM_BRANCH_ACTION:
            {
                /* special handle for bridge port with 0xffff(all LAN ports): STP status get */
                if (mgmtObj.type == TK_OAM_LEAF_BRIDGE_PORT_OBJ && mgmtObj.port == 0xffff)
                {
                    pRecvVarCont->leaf = ntohs(pRecvVarCont->leaf);
                    for (i = OAM_UNI_PORT_ID1; i <= OAM_UNI_PORT_ID4; i++)
                    {
                        tk_oam_mgmt_obj_t tmpObj;
                        memset(&tmpObj, 0, sizeof(tk_oam_mgmt_obj_t));
                        tmpObj.type = TK_OAM_LEAF_BRIDGE_PORT_OBJ;
                        tmpObj.port = i;
                        objLen = oam_build_tk_obj_instance(pOutVarCont,tmpObj);
                        pOutVarCont += objLen;
                        outPktLen += objLen;

                        pOut = (oam_var_cont_t *)pOutVarCont;
                        pOut->extBranch = pRecvVarCont->extBranch;
                        pOut->leaf = htons(pRecvVarCont->leaf);

                        get_handler = oam_find_tk_get_handler(tmpObj.type, pRecvVarCont->extBranch, pRecvVarCont->leaf);
                        if(get_handler)
                        {
                            length = get_handler(tmpObj,pRecvVarCont->extBranch, pRecvVarCont->leaf,pOutVarCont);
                            outPktLen += length + sizeof(tk_oam_var_desc_t);
                            pOutVarCont += length + sizeof(tk_oam_var_desc_t);
                        }
                        else
                        {
                            OAM_ORG_LOG( "get_func is NULL of branch 0x%02x leaf 0x%04x\r\n",pRecvVarCont->extBranch, pRecvVarCont->leaf);
                            pOut->len = TK_VAR_RSP_CODE_OK;
                            outPktLen += sizeof(oam_var_cont_t);
                            pOutVarCont += sizeof(oam_var_cont_t);
                            break;
                        }
                    }
                    pInVarCont += sizeof(tk_oam_var_desc_t);
                }
                else
                {
                    pOut->extBranch = pRecvVarCont->extBranch;
                    pOut->leaf = pRecvVarCont->leaf;

                    pRecvVarCont->leaf = ntohs(pRecvVarCont->leaf);
                    get_handler = oam_find_tk_get_handler(mgmtObj.type, pRecvVarCont->extBranch, pRecvVarCont->leaf);
                    if(get_handler)
                    {
                        length = get_handler(mgmtObj,pRecvVarCont->extBranch, pRecvVarCont->leaf,pOutVarCont);
                    }
                    else
                    {
                        OAM_ORG_LOG( "get_func is NULL of branch 0x%02x leaf 0x%04x\r\n", pRecvVarCont->extBranch, pRecvVarCont->leaf);
                        pOut->len = TK_VAR_RSP_CODE_OK;
                        length  = 1;
                    }
                    outPktLen += length + sizeof(tk_oam_var_desc_t);
                    pOutVarCont += length + sizeof(tk_oam_var_desc_t);
                    pInVarCont += sizeof(tk_oam_var_desc_t);
                }
                break;
            }

            default:
            {
                OAM_ORG_LOG("unknown TK branch 0x%02x\r\n", pRecvVarCont->extBranch);            
                objLen = oam_build_tk_var_cont(pOutVarCont, pRecvVarCont->extBranch, pRecvVarCont->leaf, TK_VAR_RSP_CODE_OK);                
                outPktLen += objLen;
                pOutVarCont += objLen;
                goto send_resp;
                break;
            }
        }

        totalContentLen -= sizeof(tk_oam_var_desc_t);
        if(totalContentLen < sizeof(tk_oam_var_desc_t))
            break;
        if(outPktLen >= OAM_MAX_OAMPDU_SIZE-1)
            break;

        pRecvVarCont = (tk_oam_var_desc_t *)pInVarCont;
        good_packet = 1;
    }

send_resp:
    oam_tk_send(OAM_PON_PORT_ID, llid, pdu, outPktLen);
    intf->org_specific_tx++;

    return outPktLen;

ignored:
    return outPktLen;
}

static oam_int32
oam_build_tk_set_resp (
        oam_llid_t llid,
        oam_uint8 *frame,
        oam_uint32 lenghth)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE];
    oam_if_t *intf = oam_intf_find(llid);
    tk_oam_pdu_t *pRecvPkt = (tk_oam_pdu_t *)frame;
    tk_oam_pdu_t *pOutPkt = (tk_oam_pdu_t *)pdu;
    tk_oam_var_cont_val_t *pRecvVarCont = NULL;
    oam_uint8 *pInVarCont = NULL;
    oam_uint8 *pOutVarCont = NULL;
    oam_uint32 inLen = 0;
    oam_uint32 totalContentLen = 0;
    oam_uint32 outPktLen = 0;
    tk_oam_mgmt_obj_t mgmtObj;
    oam_uint32 objLen = 0;
    oam_uint32 i=0;
    oam_uint32 length = 0;
    oam_uint8 good_packet = 0;
    tk_oam_set_rountine_t set_handler = NULL;

    //g_oam_tlv_num = 0;

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);
    if(!frame || lenghth <= sizeof(tk_oam_tlv_t) + sizeof(tk_oam_pdu_t) - 2)
        return 0;

    if (!intf) 
    {
        OAM_ORG_LOG( "interface not found for port 0x%x\r\n", llid);
        return 0;
    }

    /* we need to build header to pass sanity check */
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    memcpy(pOutPkt->oui, pRecvPkt->oui, OAM_OUI_LEN);
    pOutPkt->opcode = TK_OPCODE_SET_RSP;

    outPktLen =  sizeof(tk_oam_pdu_t) - 1;
    pOutVarCont = pOutPkt->data;

    totalContentLen = lenghth - sizeof(tk_oam_pdu_t) + 1;
    pRecvVarCont = (tk_oam_var_cont_val_t *)pRecvPkt->data;
    pInVarCont = (oam_uint8 *)pRecvVarCont;

    while(totalContentLen>= (sizeof(tk_oam_tlv_t)-1)){
        tk_oam_var_cont_t *pOut = (tk_oam_var_cont_t *)pOutVarCont;
        OAM_ORG_LOG("llid %d, branch 0x%x, leaf 0x%x\r\n", llid, pRecvVarCont->extBranch, ntohs(pRecvVarCont->leaf));
        switch(pRecvVarCont->extBranch)
        {
            case OAM_ATTRIB_BRANCH_NULL:
                if(!good_packet)
                    goto ignored;/*clause57 57.6.3 a)*/
                else
                    goto send_resp;

            case TK_OAM_BRANCH_NAME:
            {
                if(OAM_E_OK == oam_get_tk_obj_info((oam_uint8*)pRecvVarCont, &objLen, &mgmtObj))
                {
                    pInVarCont += objLen;
                    pRecvVarCont = (tk_oam_var_desc_t *)pInVarCont;
                    totalContentLen -= objLen;
                    OAM_ORG_LOG("get obj len %d\r\n", objLen);
                    objLen = oam_build_tk_obj_instance(pOutVarCont,mgmtObj);
                    OAM_ORG_LOG("build obj len %d\r\n", objLen);
                    outPktLen += objLen;
                    pOutVarCont += objLen;
                    continue;
                }
                else
                {
                    objLen = oam_build_tk_var_cont(pOutVarCont, pRecvVarCont->extBranch, ntohs(pRecvVarCont->leaf), TK_VAR_RSP_CODE_OK);                
                    outPktLen += objLen;
                    pOutVarCont += objLen;
                    goto send_resp;
                }
                break;
            }
            case TK_OAM_BRANCH_ATTRIB:
            case TK_OAM_BRANCH_ACTION:
            {
                pOut->extBranch = pRecvVarCont->extBranch;
                pOut->leaf = pRecvVarCont->leaf;

                pRecvVarCont->leaf = ntohs(pRecvVarCont->leaf);
                set_handler = oam_find_tk_set_handler(mgmtObj.type, pRecvVarCont->extBranch, pRecvVarCont->leaf);
                if(set_handler)
                {
                    length = set_handler(pRecvVarCont, &inLen, mgmtObj, &outPktLen, pOutVarCont);
                    OAM_ORG_LOG("set handler return length %d\r\n", length);
                }
                else
                {
                    OAM_ORG_LOG( "set_func is NULL of branch 0x%02x leaf 0x%04x\r\n",pRecvVarCont->extBranch, pRecvVarCont->leaf);
                    pOut->len = TK_VAR_RSP_CODE_OK;
                    length = 1;
                }
                outPktLen += length + sizeof(tk_oam_var_desc_t);
                pOutVarCont += length + sizeof(tk_oam_var_desc_t);

                inLen = sizeof(tk_oam_tlv_t) - 1 + pRecvVarCont->len;
                pInVarCont += inLen;
                
                OAM_ORG_LOG("outPktLen %d, inLen %d\r\n", outPktLen, inLen);
                break;
            }
            default:
            {
                OAM_ORG_LOG("unknown TK branch 0x%02x\r\n", pRecvVarCont->extBranch);
                objLen = oam_build_tk_var_cont(pOutVarCont, pRecvVarCont->extBranch, ntohs(pRecvVarCont->leaf), TK_VAR_RSP_CODE_OK);                
                outPktLen += objLen;
                pOutVarCont += objLen;
                goto send_resp;
                break;
            }
        }

        totalContentLen -= inLen;
        if(totalContentLen < (sizeof(tk_oam_tlv_t)-1))
            break;
        if(outPktLen >= OAM_MAX_OAMPDU_SIZE-1)
            break;

        pRecvVarCont = (tk_oam_var_cont_val_t *)pInVarCont;
        good_packet = 1;
        
        OAM_ORG_LOG("goodpkt \r\n");
    }

send_resp:
#ifdef HAVE_KT_OAM
    tk_oam_vlan_member_cfg_adapt(mgmtObj.port);
#endif   
    OAM_ORG_LOG("oam_tk_send llid %d \r\n", llid);
    oam_tk_send(OAM_PON_PORT_ID, llid, pdu, outPktLen);
    intf->org_specific_tx++;

    return outPktLen;

ignored:
    return outPktLen;
}

#ifdef HAVE_KT_OAM
static oam_int32
oam_build_tk_ext_info (
        oam_llid_t llid,
        oam_uint8 *frame,
        oam_uint32 lenghth)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE];
    oam_if_t *intf = oam_intf_find(llid);
    tk_oam_pdu_t *pRecvPkt = (tk_oam_pdu_t *)frame;
    tk_oam_pdu_t *pOutPkt = (tk_oam_pdu_t *)pdu;
    oam_uint8 *pOutVarCont = NULL;
    oam_uint32 outPktLen = 0;

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);
    if(!frame || lenghth <= sizeof(tk_oam_pdu_t) - 1)
        return 0;

    if (!intf) 
    {
        OAM_ORG_LOG( "\r\nat unknown port 0x%x\r\n", llid);
        return 0;
    }

    /* we need to build header to pass sanity check */
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    memcpy(pOutPkt->oui, pRecvPkt->oui, OAM_OUI_LEN);
    memcpy(&pOutPkt->opcode, &pRecvPkt->opcode, sizeof(pRecvPkt->opcode));

    outPktLen =  sizeof(tk_oam_pdu_t) - 1;
    pOutVarCont = pOutPkt->data;

    tk_oam_extended_info_t *p_tk_ext_info = (tk_oam_extended_info_t *)pOutVarCont;
    oam_tk_ext_info_init_adapt(p_tk_ext_info);
    outPktLen += sizeof(tk_oam_extended_info_t);
    outPktLen += 2;

    oam_tk_send(OAM_PON_PORT_ID, llid, pdu, outPktLen);
    intf->org_specific_tx++;

    return outPktLen;

}

static oam_int32
oam_build_tk_mc_resp (
        oam_llid_t llid,
        oam_uint8 *frame,
        oam_uint32 lenghth)
{
    oam_uint8  pdu[OAM_MAX_OAMPDU_SIZE];
    oam_if_t *intf = oam_intf_find(llid);
    tk_oam_pdu_t *pRecvPkt = (tk_oam_pdu_t *)frame;
    tk_oam_pdu_t *pOutPkt = (tk_oam_pdu_t *)pdu;
    oam_uint8 *pOutVarCont = pOutPkt->data;
    tk_mc_reg_t * pInVarCont= (tk_mc_reg_t *)pRecvPkt->data;
    tk_mc_reg_t * p_rsp = (tk_mc_reg_t *)pOutVarCont;
    oam_uint32 outPktLen = 0;

    memset(pdu, 0, OAM_MAX_OAMPDU_SIZE);
    if(!frame || lenghth <= sizeof(tk_oam_pdu_t) - 1)
        return 0;

    if (!intf) 
    {
        OAM_ORG_LOG( "\r\nat unknown port 0x%x\r\n", llid);
        return 0;
    }

    /* we need to build header to pass sanity check */
    oam_build_pdu_hdr(intf, (oam_pdu_hdr_t *)pdu, OAM_PDU_CODE_ORG_SPEC);
    memcpy(pOutPkt->oui, pRecvPkt->oui, OAM_OUI_LEN);
    pOutPkt->opcode = TK_OPCODE_MC_REG_RSP;
    outPktLen =  sizeof(tk_oam_pdu_t) - 1;

    *pOutVarCont++ = 1;
    p_rsp->mc_llid = pInVarCont->mc_llid;
    p_rsp->uc_llid = pInVarCont->uc_llid;

   // *pOutVarCont++ = 0x7f ;
   // *pOutVarCont++ = 0xff ;

   // memcpy(pOutVarCont, (pRecvPkt->data+3), sizeof(oam_uint16));
    pOutVarCont += sizeof(tk_mc_reg_t)-1;
    outPktLen += sizeof(tk_mc_reg_t);

    oam_tk_send(OAM_PON_PORT_ID, llid, pdu, outPktLen);
    intf->org_specific_tx++;

    return outPktLen;

}
#endif
/*****************************************************************************/
/* $rtn_hdr_start  handle_tk_oams                                           */
/* CATEGORY   : Device                                                       */
/* ACCESS     : public                                                       */
/* BLOCK      : General                                                      */
/* CHIP       : CS8016                                                       */
/*                                                                           */
void handle_tk_oams(
        oam_llid_t llid,
        oam_uint8 *frame,
        oam_uint32 len)
    /*                                                                           */
    /* INPUTS  : o  port - OAM port ID                                           */
    /*     o  frame - Container buffer to store the composed OAM frame           */
    /*     o  len - Size of OAM frame                                            */
    /* OUTPUTS : ----                                                            */
    /* RETURNS : ----                                                            */
    /* DESCRIPTION:                                                              */
    /* API to handler TK OAM frame                                              */
    /* $rtn_hdr_end                                                              */
    /*****************************************************************************/
{
    tk_oam_pdu_t *pTKOamFrame = (tk_oam_pdu_t *)frame;

    if(!frame || !len)
        return;

    switch(pTKOamFrame->opcode) 
    {
        case TK_OPCODE_GET:
            oam_build_tk_var_resp(llid, frame, len);
            break;
        case TK_OPCODE_SET:
            oam_build_tk_set_resp(llid, frame, len);
            break;
#ifdef HAVE_KT_OAM            
        case TK_OPCODE_EXT_INFO:
            oam_build_tk_ext_info(llid, frame, len);
            break;
        case TK_OPCODE_MC_REQ:
        case TK_OPCODE_MC_REG:
            oam_build_tk_mc_resp(llid, frame, len);
            break;            
        case TK_OPCODE_FILE_READ_REQ:
        case TK_OPCODE_FILE_WRITE_REQ:
        case TK_OPCODE_FILE_DATA:
        case TK_OPCODE_FILE_ACK:
            oam_tk_upg_build(llid, frame, len);
            break;
#endif            
        default:
            break;
    }

    return;

}

void oam_tk_init()
{
    oam_tk_adapt_init();
}
#endif
