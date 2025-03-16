#!/bin/bash
set -e

# 3. Build Proton
./build_proton.sh

# 4. (Optional) Move the built Proton to your custom Proton folder
# Suppose the build output is in "proton_build" and you want to install it to ~/Proton-eos:
mv proton_build ~/Proton-eos

echo "Proton-eos build complete and installed in ~/Proton-eos"
