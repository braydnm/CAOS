%ifndef BOOTINFO_INCLUDE
%define BOOTINFO_INCLUDE

struc multibootInfo
	.flags				resd	1
	.memoryLo			resd	1
	.memoryHi			resd	1
	.bootDevice			resd	1
	.cmdLine			resd	1
	.modsCount			resd	1
	.modsAddr			resd	1
	.syms0				resd	1
	.syms1				resd	1
	.syms2				resd	1
	.mmapLength		resd	1
	.mmapAddr			resd	1
	.drivesLength		resd	1
	.drivesAddr		resd	1
	.configTable		resd	1
	.bootloaderName	resd	1
	.apmTable			resd	1
	.vbeControlInfo	resd	1
	.vbeModeInfo		resd	1
	.vbeMode			resw	1
	.vbeInterfaceSeg	resw	1
	.vbeInterfaceOff	resw	1
	.vbeInterfaceLen	resw	1
endstruc


%endif