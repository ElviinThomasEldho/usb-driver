cmd_/home/pseudoku/Documents/usb-driver/ppause/Module.symvers := sed 's/\.ko$$/\.o/' /home/pseudoku/Documents/usb-driver/ppause/modules.order | scripts/mod/modpost -m -a  -o /home/pseudoku/Documents/usb-driver/ppause/Module.symvers -e -i Module.symvers   -T -
