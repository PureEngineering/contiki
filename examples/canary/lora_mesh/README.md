LoRa Mesh example

Description:
    Canary's LoRa mesh example using modified STMicroelectronics's Loramote
    port to Contiki. Example replaces cc2650 phy layer with the LoRa radio
    driver. By using most null drivers in the netstack we should be able to
    acheive a 6lowpan meshing network using the LoRa radio.

Directions:
    1. Flash one sensor with lora_mesh_slip to act as the receiver
       for the edge router
          - Plug node into linux box/edge router with serial->uart converter
            (i.e. ftdi). Make sure the edge router is running 6lbr's 6lowpan
            rpl edge router.

    2. Flash other nodes with lora_mesh_node
          - On start, nodes should find themselves in the mesh and
            start transmitting
          - Nodes will send periodic UDP packet with its rpl information

Comments:
    Contiki's netstack nullmac and nullrdc drivers are used resulting in no
    collision handling (nullmac) and no radio cycling (nullrdc) which should
    result in a unstable and power hungry network. Best suited for testing and ac powered nodes.
