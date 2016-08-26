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


echo "Using $CONTIKI"
rm -rf $CONTIKI/platform/loramote
rm -rf $CONTIKI/cpu/arm/stm32l1
rm -rf $CONTIKI/cpu/stm32_common
rm -rf $CONTIKI/examples/loramote


if $NO_TOOLS; then
  echo "SETUP: Did not remove tools."
  exit
fi

rm -rf $CONTIKI/tools/loramote
rm -rf $CONTIKI/tools/stm32
