# User settings
NO_TOOLS=false


# Prepare setup environment
SETUP_DIR=$(dirname -- "$(readlink -e -- "$BASH_SOURCE")")/
cd $SETUP_DIR
find . -name "*~" -type f -delete

# Check if Contiki is installed in current directory
# The install script picks the most recent edited folder when multiple installations are available
CONTIKI="$(ls -d -t $SETUP_DIR/../*contiki*/core/../ 2> /dev/null | head -1)"
if [ -z "$CONTIKI" ]; then
  echo "ERROR: Contiki installation folder not found in current directory."
  exit
fi


# Set directories
echo "Using $CONTIKI"
STM32L1_CPU=$CONTIKI/cpu/arm/stm32l1/
LORA_PLATFORM=$CONTIKI/platform/loramote/
STM32_COMMON=$CONTIKI/cpu/stm32_common/
STM32_TOOLS=$CONTIKI/tools/stm32/
LORA_TOOLS=$CONTIKI/tools/loramote/
LORA_EXAMPLES=$CONTIKI/examples/loramote/

TMP_LORAMAC=./tmp-loramac
TMP_STLINK=./tmp-stlink
TMP_DFU_UTIL=./tmp-dfu-util
TMP_OPENOCD=./tmp-openocd
TMP_LIBUSB=./tmp-libusb
TMP_AUTOCONF=./tmp-autoconf


# Integrate LoRa into Contiki
mkdir $STM32L1_CPU
mkdir $LORA_PLATFORM
mkdir $LORA_PLATFORM/LoRaMac
mkdir $LORA_PLATFORM/LoRaMac/board
mkdir $STM32_COMMON
mkdir $STM32_TOOLS
mkdir $STM32_TOOLS/serialdump
mkdir $LORA_TOOLS
mkdir $LORA_TOOLS/loramote-bootloader
mkdir $LORA_EXAMPLES

# LoRaMac 4.2 uses the STM32L1 HAL Library instead of the Standard Peripheral Library
#git clone https://github.com/Lora-net/LoRaMac-node $TMP_LORAMAC
wget https://github.com/Lora-net/LoRaMac-node/archive/v4.1.0.tar.gz
tar -xzf v4.1.0.tar.gz
rm v4.1.0.tar.gz
mv LoRaMac-node-4.1.0 $TMP_LORAMAC
mv $TMP_LORAMAC/src/boards/LoRaMote/cmsis/* $STM32L1_CPU
#mv $TMP_LORAMAC/src/boards/mcu/stm32/cmsis/* $CONTIKI/cpu/arm/common/CMSIS
rm -rf $TMP_LORAMAC/src/boards/LoRaMote/cmsis
rm -rf $TMP_LORAMAC/src/boards/mcu/stm32/cmsis
mv $TMP_LORAMAC/src/boards/mcu/stm32/STM32L1xx_StdPeriph_Driver $STM32L1_CPU
mv $TMP_LORAMAC/src/boards/mcu/stm32/STM32_USB-FS-Device_Driver $STM32_COMMON
mv $TMP_LORAMAC/src/boards/LoRaMote/* $LORA_PLATFORM/LoRaMac/board/
mv $TMP_LORAMAC/src/boards/mcu/stm32/* $STM32_COMMON
mv $TMP_LORAMAC/src/apps/BootLoader/LoRaMote/main.c $LORA_TOOLS/loramote-bootloader/loramote_bootloader.c
rm -rf $TMP_LORAMAC/src/apps
rm -rf $TMP_LORAMAC/src/boards
mv $TMP_LORAMAC/src/* $LORA_PLATFORM/LoRaMac/
mv $LORA_PLATFORM/LoRaMac/system/timer.h $LORA_PLATFORM/LoRaMac/system/loratimer.h
mv $LORA_PLATFORM/LoRaMac/system/timer.c $LORA_PLATFORM/LoRaMac/system/loratimer.c
mv $LORA_PLATFORM/LoRaMac/radio/radio.h $LORA_PLATFORM/LoRaMac/radio/loraradio.h
sed -i 's/#define MAX/#define STM32MAX/g' $STM32_COMMON/utilities.h
sed -i 's/#define MIN/#define STM32MIN/g' $STM32_COMMON/utilities.h
sed -i 's/timer.h/loratimer.h/g' $LORA_PLATFORM/LoRaMac/board/board.h
sed -i 's/radio.h/loraradio.h/g' $LORA_PLATFORM/LoRaMac/board/board.h
sed -i 's/void RTC_Alarm_IRQHandler/#pragma weak RTC_Alarm_IRQHandler\nvoid RTC_Alarm_IRQHandler/g' \
	$LORA_PLATFORM/LoRaMac/board/rtc-board.c
rm -rf $TMP_LORAMAC
cp -r $SETUP_DIR/platform/* $LORA_PLATFORM
cp $SETUP_DIR/cpu/* $STM32L1_CPU
cp $SETUP_DIR/examples/* $LORA_EXAMPLES

if $NO_TOOLS; then
  echo "SETUP: Did not install tools."
  exit
fi


# Install prerequisites
sudo apt-get install git
sudo apt-get install make
sudo apt-get install perl
sudo apt-get install libusb-1.0-0-dev
sudo apt-get install libusb-dev
sudo apt-get install pkg-config
sudo apt-get install m4
sudo apt-get install autotools-dev
sudo apt-get install autoconf
sudo apt-get install automake
sudo apt-get install libtool
sudo apt-get install texinfo
sudo apt-get install libudev-dev

# Get latest autoconf on Ubuntu 12.04 (needed to build OpenOCD)
wget http://ftp.gnu.org/gnu/autoconf/autoconf-latest.tar.gz
tar xf autoconf*
cd autoconf*
./configure
sudo make install
cd ..
rm -rf autoconf*

# Get latest libusb on Ubuntu 12.04 (needed to build OpenOCD & DFU-util)
git clone https://github.com/libusb/libusb $TMP_LIBUSB
cd $TMP_LIBUSB
./autogen.sh
./configure
sudo make install
cd ..
sudo rm -rf $TMP_LIBUSB


# Install GNU ARM Embedded Toolchain (compiler, linker & debugger)
# Version 4.9 breaks the LoRaMote bootloader application
wget https://launchpad.net/gcc-arm-embedded/4.8/4.8-2014-q3-update/+download/gcc-arm-none-eabi-4_8-2014q3-20140805-linux.tar.bz2
tar -xvf gcc-arm-none-eabi-4_8-2014q3-20140805-linux.tar.bz2
rm gcc-arm-none-eabi-4_8-2014q3-20140805-linux.tar.bz2
mv gcc-arm-none-eabi-4_8-2014q3 $STM32_TOOLS/gcc-arm-none-eabi
#sudo apt-get remove binutils-arm-none-eabi gcc-arm-none-eabi gnu-arm-embedded
#sudo add-apt-repository ppa:team-gcc-arm-embedded/ppa -y
#sudo apt-get update
#sudo apt-get install gcc-arm-none-eabi
#sudo apt-get install gdb-arm-none-eabi

# Install STLink flasher & add USB permissions
git clone https://github.com/texane/stlink $TMP_STLINK
cd $TMP_STLINK
./autogen.sh
./configure
make
sudo cp ./49-stlinkv*.rules /etc/udev/rules.d/
cd ..
mv $TMP_STLINK $STM32_TOOLS/stlink

# Install DFU-util & add USB permissions
git clone git://git.code.sf.net/p/dfu-util/dfu-util $TMP_DFU_UTIL
cd $TMP_DFU_UTIL
./autogen.sh
./configure
make
sudo cp ./doc/40-dfuse.rules /etc/udev/rules.d/
cd ..
mv $TMP_DFU_UTIL $STM32_TOOLS/dfu-util
cp $SETUP_DIR/tools/build_loramote_bootloader.sh $LORA_TOOLS/loramote-bootloader/
chmod +x $LORA_TOOLS/loramote-bootloader/build_loramote_bootloader.sh
$LORA_TOOLS/loramote-bootloader/build_loramote_bootloader.sh
#cp $SETUP_DIR/tools/binaries/loramote_bootloader.bin $LORA_TOOLS/loramote-bootloader/

# Install OpenOCD & add USB permissions
git clone git://git.code.sf.net/p/openocd/code $TMP_OPENOCD
cd $TMP_OPENOCD
./bootstrap
./configure
make
sudo cp ./contrib/99-openocd.rules /etc/udev/rules.d/
cd ..
mv $TMP_OPENOCD $STM32_TOOLS/openocd
cp $SETUP_DIR/tools/loramote.cfg $LORA_TOOLS

# Install serialdump
cp $SETUP_DIR/tools/motelist-lora $LORA_TOOLS
cd $STM32_TOOLS/serialdump
wget https://raw.githubusercontent.com/contiki-os/contiki/master/tools/sky/Makefile
wget https://raw.githubusercontent.com/contiki-os/contiki/master/tools/sky/serialdump.c
make
cd ..

# Enable new USB permissions & ignore STLink USB implementation
sudo udevadm control --reload-rules
sudo udevadm trigger
sudo modprobe -r usb-storage && sudo modprobe usb-storage quirks=483:3744:i


# Replace faulty binaries
#cp $SETUP_DIR/tools/binaries/dfu-util $STM32_TOOLS/dfu-util/src/
#cp $SETUP_DIR/tools/binaries/openocd $STM32_TOOLS/openocd/src/
#cp $SETUP_DIR/tools/binaries/st-flash $STM32_TOOLS/stlink/

