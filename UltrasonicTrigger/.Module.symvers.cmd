cmd_/home/pseudoku/Desktop/UltrasonicTrigger/Module.symvers := sed 's/\.ko$$/\.o/' /home/pseudoku/Desktop/UltrasonicTrigger/modules.order | scripts/mod/modpost -m -a  -o /home/pseudoku/Desktop/UltrasonicTrigger/Module.symvers -e -i Module.symvers   -T -
