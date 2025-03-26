#include "scan.h"
#include <python3.13/pylifecycle.h>
#include <bluetooth/bluetooth.h>
#include <python3.13/Python.h>

int main(){
	bdaddr_t addr;
	char message;
	scan_devices(&addr);
}
