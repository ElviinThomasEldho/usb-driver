#!/bin/sh
echo -ne '\033c\033]0;USB Read Write\a'
base_path="$(dirname "$(realpath "$0")")"
"$base_path/USBReadWrite.x86_64" "$@"
