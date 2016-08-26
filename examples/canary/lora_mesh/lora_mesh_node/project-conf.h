
#ifndef PROJECT_CONF_H
#define PROJECT_CONF_H

//netstack drivers
#define NETSTACK_CONF_MAC			    nullmac_driver //pass through driver
#define NETSTACK_CONF_RDC		     	nullrdc_driver //no sleeping, calls framer
#define NETSTACK_CONF_FRAMER			framer_802154 // 802154 protocol framer
#define NETSTACK_CONF_RADIO		    lora_radio_driver // lora radio phy layer

//6lowpan network stack
#define LINKADDR_CONF_SIZE			8
#define NETSTACK_CONF_NETWORK			sicslowpan_driver //use the 6lowpan stack
#define SICSLOWPAN_CONF_MAC_MAX_PAYLOAD	NETSTACK_RADIO_MAX_PAYLOAD_LEN
#define SICSLOWPAN_CONF_COMPRESSION		SICSLOWPAN_COMPRESSION_HC06
#define SICSLOWPAN_CONF_FRAG			1

//rpl meshing
#define UIP_CONF_ROUTER			1
#define UIP_CONF_IPV6_RPL			1


#endif // PROJECT_CONF_H
