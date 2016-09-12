LoRa Mesh example

Description:
    rpl-udp is a contiki provided example, only changes are in the project-conf
    file and disabling the button on the udp-server file.

    Canary's LoRa mesh example using modified STMicroelectronics's Loramote
    port to Contiki. Example replaces cc2650 phy layer with the LoRa radio
    driver.

Directions:
    1. Use command "make all TARGET=srf06-cc26xx BOARD=canary/cc2650" to build both udp-client and udp-server. Flash one node with server and any number of nodes with client.
