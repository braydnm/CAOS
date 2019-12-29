.section .symbols

.extern IRQ_END
.type IRQ_END, @function

.extern IRQ_START
.type IRQ_START, @function

.extern __assert_func
.type __assert_func, @function

.extern __enabled
.type __enabled, @function

.extern __kmalloc
.type __kmalloc, @function

.extern __print_used_mem
.type __print_used_mem, @function

.extern __x86.get_pc_thunk.ax
.type __x86.get_pc_thunk.ax, @function

.extern __x86.get_pc_thunk.bx
.type __x86.get_pc_thunk.bx, @function

.extern __x86.get_pc_thunk.cx
.type __x86.get_pc_thunk.cx, @function

.extern __x86.get_pc_thunk.dx
.type __x86.get_pc_thunk.dx, @function

.extern _itoa
.type _itoa, @function

.extern _kprint
.type _kprint, @function

.extern _kprintf
.type _kprintf, @function

.extern _pci_devices
.type _pci_devices, @function

.extern _pci_vendors
.type _pci_vendors, @function

.extern _timer_drift
.type _timer_drift, @function

.extern addInputHandler
.type addInputHandler, @function

.extern addInterruptHandler
.type addInterruptHandler, @function

.extern alloc_dma_frame
.type alloc_dma_frame, @function

.extern alloc_frame
.type alloc_frame, @function

.extern allocate_block
.type allocate_block, @function

.extern allocate_inode
.type allocate_inode, @function

.extern allocate_inode_block
.type allocate_inode_block, @function

.extern appendChar
.type appendChar, @function

.extern appendStr
.type appendStr, @function

.extern atoi
.type atoi, @function

.extern backspace
.type backspace, @function

.extern bootInfo
.type bootInfo, @function

.extern boot_time
.type boot_time, @function

.extern cache_flush_dirty
.type cache_flush_dirty, @function

.extern calloc
.type calloc, @function

.extern canonicalize_path
.type canonicalize_path, @function

.extern capitalize
.type capitalize, @function

.extern caps
.type caps, @function

.extern ceil
.type ceil, @function

.extern charsOnLine
.type charsOnLine, @function

.extern chmod_fs
.type chmod_fs, @function

.extern chown_fs
.type chown_fs, @function

.extern clearScreen
.type clearScreen, @function

.extern clear_frame
.type clear_frame, @function

.extern clone_fs
.type clone_fs, @function

.extern close_ext2
.type close_ext2, @function

.extern close_fs
.type close_fs, @function

.extern close_pipe
.type close_pipe, @function

.extern cmos_dump
.type cmos_dump, @function

.extern colors
.type colors, @function

.extern contains
.type contains, @function

.extern copy_multiboot
.type copy_multiboot, @function

.extern copy_page_dir
.type copy_page_dir, @function

.extern copy_page_physical
.type copy_page_physical, @function

.extern count
.type count, @function

.extern create_entry
.type create_entry, @function

.extern create_ext2
.type create_ext2, @function

.extern create_file_fs
.type create_file_fs, @function

.extern ctrl
.type ctrl, @function

.extern current_directory
.type current_directory, @function

.extern cwd
.type cwd, @function

.extern data
.type data, @function

.extern deleteChar
.type deleteChar, @function

.extern device_get
.type device_get, @function

.extern device_register
.type device_register, @function

.extern direntry_ext2
.type direntry_ext2, @function

.extern do_multiboot
.type do_multiboot, @function

.extern dump_multiboot
.type dump_multiboot, @function

.extern endswith
.type endswith, @function

.extern errno
.type errno, @function

.extern error0
.type error0, @function

.extern error1
.type error1, @function

.extern error10
.type error10, @function

.extern error11
.type error11, @function

.extern error12
.type error12, @function

.extern error13
.type error13, @function

.extern error14
.type error14, @function

.extern error15
.type error15, @function

.extern error16
.type error16, @function

.extern error17
.type error17, @function

.extern error18
.type error18, @function

.extern error19
.type error19, @function

.extern error2
.type error2, @function

.extern error20
.type error20, @function

.extern error21
.type error21, @function

.extern error22
.type error22, @function

.extern error23
.type error23, @function

.extern error24
.type error24, @function

.extern error25
.type error25, @function

.extern error26
.type error26, @function

.extern error27
.type error27, @function

.extern error28
.type error28, @function

.extern error29
.type error29, @function

.extern error3
.type error3, @function

.extern error30
.type error30, @function

.extern error31
.type error31, @function

.extern error4
.type error4, @function

.extern error5
.type error5, @function

.extern error6
.type error6, @function

.extern error7
.type error7, @function

.extern error8
.type error8, @function

.extern error9
.type error9, @function

.extern errorHandler
.type errorHandler, @function

.extern exception_messages
.type exception_messages, @function

.extern ext2_fs_mount
.type ext2_fs_mount, @function

.extern ext2_initialize
.type ext2_initialize, @function

.extern ext2_root
.type ext2_root, @function

.extern ext2_sync
.type ext2_sync, @function

.extern finddir_ext2
.type finddir_ext2, @function

.extern finddir_fs
.type finddir_fs, @function

.extern first_frame
.type first_frame, @function

.extern first_n_frames
.type first_n_frames, @function

.extern floor
.type floor, @function

.extern free
.type free, @function

.extern free_frame
.type free_frame, @function

.extern fs_root
.type fs_root, @function

.extern fs_tree
.type fs_tree, @function

.extern fs_types
.type fs_types, @function

.extern gdt
.type gdt, @function

.extern gdtInstall
.type gdtInstall, @function

.extern gdtPtr
.type gdtPtr, @function

.extern gdtSetGate
.type gdtSetGate, @function

.extern gdt_flush
.type gdt_flush, @function

.extern getCursorLocation
.type getCursorLocation, @function

.extern getLocation
.type getLocation, @function

.extern getLocationCol
.type getLocationCol, @function

.extern getLocationRow
.type getLocationRow, @function

.extern get_block_number
.type get_block_number, @function

.extern get_cache_time
.type get_cache_time, @function

.extern get_date
.type get_date, @function

.extern get_mount_point
.type get_mount_point, @function

.extern get_nframes
.type get_nframes, @function

.extern get_num_of_devices
.type get_num_of_devices, @function

.extern get_page
.type get_page, @function

.extern get_time
.type get_time, @function

.extern gettimeofday
.type gettimeofday, @function

.extern handleCtrlPress
.type handleCtrlPress, @function

.extern handleSpecialPress
.type handleSpecialPress, @function

.extern handleUserInput
.type handleUserInput, @function

.extern hashmap_create
.type hashmap_create, @function

.extern hashmap_create_int
.type hashmap_create_int, @function

.extern hashmap_free
.type hashmap_free, @function

.extern hashmap_get
.type hashmap_get, @function

.extern hashmap_has
.type hashmap_has, @function

.extern hashmap_int_comp
.type hashmap_int_comp, @function

.extern hashmap_int_dupe
.type hashmap_int_dupe, @function

.extern hashmap_int_hash
.type hashmap_int_hash, @function

.extern hashmap_is_empty
.type hashmap_is_empty, @function

.extern hashmap_keys
.type hashmap_keys, @function

.extern hashmap_remove
.type hashmap_remove, @function

.extern hashmap_set
.type hashmap_set, @function

.extern hashmap_string_comp
.type hashmap_string_comp, @function

.extern hashmap_string_dupe
.type hashmap_string_dupe, @function

.extern hashmap_string_hash
.type hashmap_string_hash, @function

.extern hashmap_values
.type hashmap_values, @function

.extern heap_install
.type heap_install, @function

.extern heap_online
.type heap_online, @function

.extern hexToAscii
.type hexToAscii, @function

.extern hexToAsciiMap
.type hexToAsciiMap, @function

.extern idtLoad
.type idtLoad, @function

.extern init
.type init, @function

.extern initInterruptHandling
.type initInterruptHandling, @function

.extern initKeyboard
.type initKeyboard, @function

.extern init_paging
.type init_paging, @function

.extern initial_esp
.type initial_esp, @function

.extern inode_read_block
.type inode_read_block, @function

.extern inode_write_block
.type inode_write_block, @function

.extern inportb
.type inportb, @function

.extern inportl
.type inportl, @function

.extern inports
.type inports, @function

.extern inportsm
.type inportsm, @function

.extern inportw
.type inportw, @function

.extern inputHandlerInit
.type inputHandlerInit, @function

.extern inputHandlers
.type inputHandlers, @function

.extern install_modules
.type install_modules, @function

.extern interruptHandlers
.type interruptHandlers, @function

.extern interruptServiceRoutine0
.type interruptServiceRoutine0, @function

.extern interruptServiceRoutine1
.type interruptServiceRoutine1, @function

.extern interruptServiceRoutine10
.type interruptServiceRoutine10, @function

.extern interruptServiceRoutine11
.type interruptServiceRoutine11, @function

.extern interruptServiceRoutine12
.type interruptServiceRoutine12, @function

.extern interruptServiceRoutine13
.type interruptServiceRoutine13, @function

.extern interruptServiceRoutine14
.type interruptServiceRoutine14, @function

.extern interruptServiceRoutine15
.type interruptServiceRoutine15, @function

.extern interruptServiceRoutine2
.type interruptServiceRoutine2, @function

.extern interruptServiceRoutine3
.type interruptServiceRoutine3, @function

.extern interruptServiceRoutine4
.type interruptServiceRoutine4, @function

.extern interruptServiceRoutine5
.type interruptServiceRoutine5, @function

.extern interruptServiceRoutine6
.type interruptServiceRoutine6, @function

.extern interruptServiceRoutine7
.type interruptServiceRoutine7, @function

.extern interruptServiceRoutine8
.type interruptServiceRoutine8, @function

.extern interruptServiceRoutine9
.type interruptServiceRoutine9, @function

.extern interruptServiceRoutineHandler
.type interruptServiceRoutineHandler, @function

.extern interrupts
.type interrupts, @function

.extern intro
.type intro, @function

.extern ioctl_fs
.type ioctl_fs, @function

.extern is_update_in_progress
.type is_update_in_progress, @function

.extern itoa
.type itoa, @function

.extern kernelSize
.type kernelSize, @function

.extern kernel_base
.type kernel_base, @function

.extern kernel_dir
.type kernel_dir, @function

.extern kernel_end
.type kernel_end, @function

.extern kernel_symbols_end
.type kernel_symbols_end, @function

.extern kernel_symbols_start
.type kernel_symbols_start, @function

.extern kmain
.type kmain, @function

.extern kmalloc
.type kmalloc, @function

.extern kmalloc_p
.type kmalloc_p, @function

.extern kopen
.type kopen, @function

.extern kopen_recur
.type kopen_recur, @function

.extern kprint
.type kprint, @function

.extern kprintCentered
.type kprintCentered, @function

.extern kprintPos
.type kprintPos, @function

.extern kprintf
.type kprintf, @function

.extern kprintfCentered
.type kprintfCentered, @function

.extern kvmalloc
.type kvmalloc, @function

.extern kvmalloc_p
.type kvmalloc_p, @function

.extern lfind
.type lfind, @function

.extern liballoc_alloc
.type liballoc_alloc, @function

.extern liballoc_free
.type liballoc_free, @function

.extern liballoc_lock
.type liballoc_lock, @function

.extern liballoc_unlock
.type liballoc_unlock, @function

.extern list_append
.type list_append, @function

.extern list_append_after
.type list_append_after, @function

.extern list_copy
.type list_copy, @function

.extern list_create
.type list_create, @function

.extern list_delete
.type list_delete, @function

.extern list_dequeue
.type list_dequeue, @function

.extern list_destroy
.type list_destroy, @function

.extern list_find
.type list_find, @function

.extern list_free
.type list_free, @function

.extern list_get
.type list_get, @function

.extern list_insert
.type list_insert, @function

.extern list_insert_after
.type list_insert_after, @function

.extern list_merge
.type list_merge, @function

.extern list_pop
.type list_pop, @function

.extern list_remove
.type list_remove, @function

.extern make_pipe
.type make_pipe, @function

.extern malloc
.type malloc, @function

.extern map_vfs_directory
.type map_vfs_directory, @function

.extern mark_region
.type mark_region, @function

.extern mboot_ptr
.type mboot_ptr, @function

.extern mem_total
.type mem_total, @function

.extern mem_used
.type mem_used, @function

.extern memcpy
.type memcpy, @function

.extern memmove
.type memmove, @function

.extern memset
.type memset, @function

.extern messageLength
.type messageLength, @function

.extern mkdir_ext2
.type mkdir_ext2, @function

.extern mkdir_fs
.type mkdir_fs, @function

.extern mm_alloc_pages
.type mm_alloc_pages, @function

.extern mm_free_pages
.type mm_free_pages, @function

.extern module_direct_load
.type module_direct_load, @function

.extern module_load
.type module_load, @function

.extern modules_install
.type modules_install, @function

.extern mount_ext2
.type mount_ext2, @function

.extern node_from_file
.type node_from_file, @function

.extern now
.type now, @function

.extern open_ext2
.type open_ext2, @function

.extern open_fs
.type open_fs, @function

.extern open_pipe
.type open_pipe, @function

.extern outportb
.type outportb, @function

.extern outportl
.type outportl, @function

.extern outports
.type outports, @function

.extern outportsm
.type outportsm, @function

.extern page_fault
.type page_fault, @function

.extern paging_fini
.type paging_fini, @function

.extern pci_device_lookup
.type pci_device_lookup, @function

.extern pci_find_type
.type pci_find_type, @function

.extern pci_get_interrupt
.type pci_get_interrupt, @function

.extern pci_read_field
.type pci_read_field, @function

.extern pci_remap
.type pci_remap, @function

.extern pci_scan
.type pci_scan, @function

.extern pci_scan_bus
.type pci_scan_bus, @function

.extern pci_scan_func
.type pci_scan_func, @function

.extern pci_scan_hit
.type pci_scan_hit, @function

.extern pci_scan_slot
.type pci_scan_slot, @function

.extern pci_vendor_lookup
.type pci_vendor_lookup, @function

.extern pci_write_field
.type pci_write_field, @function

.extern pic_send_eoi
.type pic_send_eoi, @function

.extern pip_unsize
.type pip_unsize, @function

.extern pipe_available
.type pipe_available, @function

.extern pipe_check
.type pipe_check, @function

.extern pipe_increment_read
.type pipe_increment_read, @function

.extern pipe_increment_read_amount
.type pipe_increment_read_amount, @function

.extern pipe_increment_write
.type pipe_increment_write, @function

.extern pipe_increment_write_amount
.type pipe_increment_write_amount, @function

.extern pipe_size
.type pipe_size, @function

.extern pipe_unread
.type pipe_unread, @function

.extern plp
.type plp, @function

.extern prealloc_start
.type prealloc_start, @function

.extern printChar
.type printChar, @function

.extern prompt
.type prompt, @function

.extern qsort
.type qsort, @function

.extern ramdisk
.type ramdisk, @function

.extern ramdisk_mount
.type ramdisk_mount, @function

.extern read_block
.type read_block, @function

.extern read_cmos
.type read_cmos, @function

.extern read_eip
.type read_eip, @function

.extern read_ext2
.type read_ext2, @function

.extern read_fs
.type read_fs, @function

.extern read_inode
.type read_inode, @function

.extern read_pipe
.type read_pipe, @function

.extern readdir_ext2
.type readdir_ext2, @function

.extern readdir_fs
.type readdir_fs, @function

.extern readlink_ext2
.type readlink_ext2, @function

.extern readlink_fs
.type readlink_fs, @function

.extern realloc
.type realloc, @function

.extern refresh_inode
.type refresh_inode, @function

.extern reg
.type reg, @function

.extern relative_time
.type relative_time, @function

.extern reverse
.type reverse, @function

.extern rewrite_superblock
.type rewrite_superblock, @function

.extern scancodeToAscii
.type scancodeToAscii, @function

.extern scancodeToName
.type scancodeToName, @function

.extern schedule_no_irq
.type schedule_no_irq, @function

.extern screenBuffer
.type screenBuffer, @function

.extern scroll
.type scroll, @function

.extern secs_of_month
.type secs_of_month, @function

.extern secs_of_years
.type secs_of_years, @function

.extern selectcheck_fs
.type selectcheck_fs, @function

.extern selectwait_fs
.type selectwait_fs, @function

.extern serialLog
.type serialLog, @function

.extern serial_enable
.type serial_enable, @function

.extern serial_install
.type serial_install, @function

.extern serial_rcvd
.type serial_rcvd, @function

.extern serial_read_byte
.type serial_read_byte, @function

.extern serial_read_byte_async
.type serial_read_byte_async, @function

.extern serial_send_char
.type serial_send_char, @function

.extern serial_send_data
.type serial_send_data, @function

.extern serial_transmit_empty
.type serial_transmit_empty, @function

.extern setBackground
.type setBackground, @function

.extern setColor
.type setColor, @function

.extern setCursorLocation
.type setCursorLocation, @function

.extern setInterrupt
.type setInterrupt, @function

.extern setInterruptGate
.type setInterruptGate, @function

.extern setKernelStack
.type setKernelStack, @function

.extern setText
.type setText, @function

.extern set_block_number
.type set_block_number, @function

.extern set_frame
.type set_frame, @function

.extern set_nframes
.type set_nframes, @function

.extern setup_modules
.type setup_modules, @function

.extern shift
.type shift, @function

.extern snprintf
.type snprintf, @function

.extern split
.type split, @function

.extern start
.type start, @function

.extern startswith
.type startswith, @function

.extern strbackspace
.type strbackspace, @function

.extern strchr
.type strchr, @function

.extern strchrnul
.type strchrnul, @function

.extern strcmp
.type strcmp, @function

.extern strcpy
.type strcpy, @function

.extern strcpy_s
.type strcpy_s, @function

.extern strcspn
.type strcspn, @function

.extern strdup
.type strdup, @function

.extern strlen
.type strlen, @function

.extern strncmp
.type strncmp, @function

.extern strpbrk
.type strpbrk, @function

.extern strspn
.type strspn, @function

.extern strstr
.type strstr, @function

.extern strtok
.type strtok, @function

.extern strtok_r
.type strtok_r, @function

.extern switch_page_dir
.type switch_page_dir, @function

.extern symlink_ext2
.type symlink_ext2, @function

.extern symlink_fs
.type symlink_fs, @function

.extern syscall
.type syscall, @function

.extern syscallRoutine
.type syscallRoutine, @function

.extern test_frame
.type test_frame, @function

.extern timer_drift
.type timer_drift, @function

.extern timer_handler
.type timer_handler, @function

.extern timer_install
.type timer_install, @function

.extern timer_phase
.type timer_phase, @function

.extern timer_subticks
.type timer_subticks, @function

.extern timer_ticks
.type timer_ticks, @function

.extern tokenize
.type tokenize, @function

.extern tree_break_off
.type tree_break_off, @function

.extern tree_count_children
.type tree_count_children, @function

.extern tree_create
.type tree_create, @function

.extern tree_destroy
.type tree_destroy, @function

.extern tree_find
.type tree_find, @function

.extern tree_find_parent
.type tree_find_parent, @function

.extern tree_free
.type tree_free, @function

.extern tree_node_create
.type tree_node_create, @function

.extern tree_node_destroy
.type tree_node_destroy, @function

.extern tree_node_find
.type tree_node_find, @function

.extern tree_node_find_parent
.type tree_node_find_parent, @function

.extern tree_node_free
.type tree_node_free, @function

.extern tree_node_insert_child
.type tree_node_insert_child, @function

.extern tree_node_insert_child_node
.type tree_node_insert_child_node, @function

.extern tree_node_parent_remove
.type tree_node_parent_remove, @function

.extern tree_node_remove
.type tree_node_remove, @function

.extern tree_remove
.type tree_remove, @function

.extern tree_remove_reparent_root
.type tree_remove_reparent_root, @function

.extern tree_set_root
.type tree_set_root, @function

.extern tssEntry
.type tssEntry, @function

.extern tss_flush
.type tss_flush, @function

.extern unlink_ext2
.type unlink_ext2, @function

.extern unlink_fs
.type unlink_fs, @function

.extern vasprintf
.type vasprintf, @function

.extern vfs_install
.type vfs_install, @function

.extern vfs_mount
.type vfs_mount, @function

.extern vfs_mount_type
.type vfs_mount_type, @function

.extern vfs_register
.type vfs_register, @function

.extern virtual_to_physical
.type virtual_to_physical, @function

.extern write_block
.type write_block, @function

.extern write_ext2
.type write_ext2, @function

.extern write_fs
.type write_fs, @function

.extern write_inode
.type write_inode, @function

.extern write_inode_buffer
.type write_inode_buffer, @function

.extern write_pipe
.type write_pipe, @function

.global kernel_symbols_start
kernel_symbols_start:

.long IRQ_END
.asciz "IRQ_END"

.long IRQ_START
.asciz "IRQ_START"

.long __assert_func
.asciz "__assert_func"

.long __enabled
.asciz "__enabled"

.long __kmalloc
.asciz "__kmalloc"

.long __print_used_mem
.asciz "__print_used_mem"

.long __x86.get_pc_thunk.ax
.asciz "__x86.get_pc_thunk.ax"

.long __x86.get_pc_thunk.bx
.asciz "__x86.get_pc_thunk.bx"

.long __x86.get_pc_thunk.cx
.asciz "__x86.get_pc_thunk.cx"

.long __x86.get_pc_thunk.dx
.asciz "__x86.get_pc_thunk.dx"

.long _itoa
.asciz "_itoa"

.long _kprint
.asciz "_kprint"

.long _kprintf
.asciz "_kprintf"

.long _pci_devices
.asciz "_pci_devices"

.long _pci_vendors
.asciz "_pci_vendors"

.long _timer_drift
.asciz "_timer_drift"

.long addInputHandler
.asciz "addInputHandler"

.long addInterruptHandler
.asciz "addInterruptHandler"

.long alloc_dma_frame
.asciz "alloc_dma_frame"

.long alloc_frame
.asciz "alloc_frame"

.long allocate_block
.asciz "allocate_block"

.long allocate_inode
.asciz "allocate_inode"

.long allocate_inode_block
.asciz "allocate_inode_block"

.long appendChar
.asciz "appendChar"

.long appendStr
.asciz "appendStr"

.long atoi
.asciz "atoi"

.long backspace
.asciz "backspace"

.long bootInfo
.asciz "bootInfo"

.long boot_time
.asciz "boot_time"

.long cache_flush_dirty
.asciz "cache_flush_dirty"

.long calloc
.asciz "calloc"

.long canonicalize_path
.asciz "canonicalize_path"

.long capitalize
.asciz "capitalize"

.long caps
.asciz "caps"

.long ceil
.asciz "ceil"

.long charsOnLine
.asciz "charsOnLine"

.long chmod_fs
.asciz "chmod_fs"

.long chown_fs
.asciz "chown_fs"

.long clearScreen
.asciz "clearScreen"

.long clear_frame
.asciz "clear_frame"

.long clone_fs
.asciz "clone_fs"

.long close_ext2
.asciz "close_ext2"

.long close_fs
.asciz "close_fs"

.long close_pipe
.asciz "close_pipe"

.long cmos_dump
.asciz "cmos_dump"

.long colors
.asciz "colors"

.long contains
.asciz "contains"

.long copy_multiboot
.asciz "copy_multiboot"

.long copy_page_dir
.asciz "copy_page_dir"

.long copy_page_physical
.asciz "copy_page_physical"

.long count
.asciz "count"

.long create_entry
.asciz "create_entry"

.long create_ext2
.asciz "create_ext2"

.long create_file_fs
.asciz "create_file_fs"

.long ctrl
.asciz "ctrl"

.long current_directory
.asciz "current_directory"

.long cwd
.asciz "cwd"

.long data
.asciz "data"

.long deleteChar
.asciz "deleteChar"

.long device_get
.asciz "device_get"

.long device_register
.asciz "device_register"

.long direntry_ext2
.asciz "direntry_ext2"

.long do_multiboot
.asciz "do_multiboot"

.long dump_multiboot
.asciz "dump_multiboot"

.long endswith
.asciz "endswith"

.long errno
.asciz "errno"

.long error0
.asciz "error0"

.long error1
.asciz "error1"

.long error10
.asciz "error10"

.long error11
.asciz "error11"

.long error12
.asciz "error12"

.long error13
.asciz "error13"

.long error14
.asciz "error14"

.long error15
.asciz "error15"

.long error16
.asciz "error16"

.long error17
.asciz "error17"

.long error18
.asciz "error18"

.long error19
.asciz "error19"

.long error2
.asciz "error2"

.long error20
.asciz "error20"

.long error21
.asciz "error21"

.long error22
.asciz "error22"

.long error23
.asciz "error23"

.long error24
.asciz "error24"

.long error25
.asciz "error25"

.long error26
.asciz "error26"

.long error27
.asciz "error27"

.long error28
.asciz "error28"

.long error29
.asciz "error29"

.long error3
.asciz "error3"

.long error30
.asciz "error30"

.long error31
.asciz "error31"

.long error4
.asciz "error4"

.long error5
.asciz "error5"

.long error6
.asciz "error6"

.long error7
.asciz "error7"

.long error8
.asciz "error8"

.long error9
.asciz "error9"

.long errorHandler
.asciz "errorHandler"

.long exception_messages
.asciz "exception_messages"

.long ext2_fs_mount
.asciz "ext2_fs_mount"

.long ext2_initialize
.asciz "ext2_initialize"

.long ext2_root
.asciz "ext2_root"

.long ext2_sync
.asciz "ext2_sync"

.long finddir_ext2
.asciz "finddir_ext2"

.long finddir_fs
.asciz "finddir_fs"

.long first_frame
.asciz "first_frame"

.long first_n_frames
.asciz "first_n_frames"

.long floor
.asciz "floor"

.long free
.asciz "free"

.long free_frame
.asciz "free_frame"

.long fs_root
.asciz "fs_root"

.long fs_tree
.asciz "fs_tree"

.long fs_types
.asciz "fs_types"

.long gdt
.asciz "gdt"

.long gdtInstall
.asciz "gdtInstall"

.long gdtPtr
.asciz "gdtPtr"

.long gdtSetGate
.asciz "gdtSetGate"

.long gdt_flush
.asciz "gdt_flush"

.long getCursorLocation
.asciz "getCursorLocation"

.long getLocation
.asciz "getLocation"

.long getLocationCol
.asciz "getLocationCol"

.long getLocationRow
.asciz "getLocationRow"

.long get_block_number
.asciz "get_block_number"

.long get_cache_time
.asciz "get_cache_time"

.long get_date
.asciz "get_date"

.long get_mount_point
.asciz "get_mount_point"

.long get_nframes
.asciz "get_nframes"

.long get_num_of_devices
.asciz "get_num_of_devices"

.long get_page
.asciz "get_page"

.long get_time
.asciz "get_time"

.long gettimeofday
.asciz "gettimeofday"

.long handleCtrlPress
.asciz "handleCtrlPress"

.long handleSpecialPress
.asciz "handleSpecialPress"

.long handleUserInput
.asciz "handleUserInput"

.long hashmap_create
.asciz "hashmap_create"

.long hashmap_create_int
.asciz "hashmap_create_int"

.long hashmap_free
.asciz "hashmap_free"

.long hashmap_get
.asciz "hashmap_get"

.long hashmap_has
.asciz "hashmap_has"

.long hashmap_int_comp
.asciz "hashmap_int_comp"

.long hashmap_int_dupe
.asciz "hashmap_int_dupe"

.long hashmap_int_hash
.asciz "hashmap_int_hash"

.long hashmap_is_empty
.asciz "hashmap_is_empty"

.long hashmap_keys
.asciz "hashmap_keys"

.long hashmap_remove
.asciz "hashmap_remove"

.long hashmap_set
.asciz "hashmap_set"

.long hashmap_string_comp
.asciz "hashmap_string_comp"

.long hashmap_string_dupe
.asciz "hashmap_string_dupe"

.long hashmap_string_hash
.asciz "hashmap_string_hash"

.long hashmap_values
.asciz "hashmap_values"

.long heap_install
.asciz "heap_install"

.long heap_online
.asciz "heap_online"

.long hexToAscii
.asciz "hexToAscii"

.long hexToAsciiMap
.asciz "hexToAsciiMap"

.long idtLoad
.asciz "idtLoad"

.long init
.asciz "init"

.long initInterruptHandling
.asciz "initInterruptHandling"

.long initKeyboard
.asciz "initKeyboard"

.long init_paging
.asciz "init_paging"

.long initial_esp
.asciz "initial_esp"

.long inode_read_block
.asciz "inode_read_block"

.long inode_write_block
.asciz "inode_write_block"

.long inportb
.asciz "inportb"

.long inportl
.asciz "inportl"

.long inports
.asciz "inports"

.long inportsm
.asciz "inportsm"

.long inportw
.asciz "inportw"

.long inputHandlerInit
.asciz "inputHandlerInit"

.long inputHandlers
.asciz "inputHandlers"

.long install_modules
.asciz "install_modules"

.long interruptHandlers
.asciz "interruptHandlers"

.long interruptServiceRoutine0
.asciz "interruptServiceRoutine0"

.long interruptServiceRoutine1
.asciz "interruptServiceRoutine1"

.long interruptServiceRoutine10
.asciz "interruptServiceRoutine10"

.long interruptServiceRoutine11
.asciz "interruptServiceRoutine11"

.long interruptServiceRoutine12
.asciz "interruptServiceRoutine12"

.long interruptServiceRoutine13
.asciz "interruptServiceRoutine13"

.long interruptServiceRoutine14
.asciz "interruptServiceRoutine14"

.long interruptServiceRoutine15
.asciz "interruptServiceRoutine15"

.long interruptServiceRoutine2
.asciz "interruptServiceRoutine2"

.long interruptServiceRoutine3
.asciz "interruptServiceRoutine3"

.long interruptServiceRoutine4
.asciz "interruptServiceRoutine4"

.long interruptServiceRoutine5
.asciz "interruptServiceRoutine5"

.long interruptServiceRoutine6
.asciz "interruptServiceRoutine6"

.long interruptServiceRoutine7
.asciz "interruptServiceRoutine7"

.long interruptServiceRoutine8
.asciz "interruptServiceRoutine8"

.long interruptServiceRoutine9
.asciz "interruptServiceRoutine9"

.long interruptServiceRoutineHandler
.asciz "interruptServiceRoutineHandler"

.long interrupts
.asciz "interrupts"

.long intro
.asciz "intro"

.long ioctl_fs
.asciz "ioctl_fs"

.long is_update_in_progress
.asciz "is_update_in_progress"

.long itoa
.asciz "itoa"

.long kernelSize
.asciz "kernelSize"

.long kernel_base
.asciz "kernel_base"

.long kernel_dir
.asciz "kernel_dir"

.long kernel_end
.asciz "kernel_end"

.long kernel_symbols_end
.asciz "kernel_symbols_end"

.long kernel_symbols_start
.asciz "kernel_symbols_start"

.long kmain
.asciz "kmain"

.long kmalloc
.asciz "kmalloc"

.long kmalloc_p
.asciz "kmalloc_p"

.long kopen
.asciz "kopen"

.long kopen_recur
.asciz "kopen_recur"

.long kprint
.asciz "kprint"

.long kprintCentered
.asciz "kprintCentered"

.long kprintPos
.asciz "kprintPos"

.long kprintf
.asciz "kprintf"

.long kprintfCentered
.asciz "kprintfCentered"

.long kvmalloc
.asciz "kvmalloc"

.long kvmalloc_p
.asciz "kvmalloc_p"

.long lfind
.asciz "lfind"

.long liballoc_alloc
.asciz "liballoc_alloc"

.long liballoc_free
.asciz "liballoc_free"

.long liballoc_lock
.asciz "liballoc_lock"

.long liballoc_unlock
.asciz "liballoc_unlock"

.long list_append
.asciz "list_append"

.long list_append_after
.asciz "list_append_after"

.long list_copy
.asciz "list_copy"

.long list_create
.asciz "list_create"

.long list_delete
.asciz "list_delete"

.long list_dequeue
.asciz "list_dequeue"

.long list_destroy
.asciz "list_destroy"

.long list_find
.asciz "list_find"

.long list_free
.asciz "list_free"

.long list_get
.asciz "list_get"

.long list_insert
.asciz "list_insert"

.long list_insert_after
.asciz "list_insert_after"

.long list_merge
.asciz "list_merge"

.long list_pop
.asciz "list_pop"

.long list_remove
.asciz "list_remove"

.long make_pipe
.asciz "make_pipe"

.long malloc
.asciz "malloc"

.long map_vfs_directory
.asciz "map_vfs_directory"

.long mark_region
.asciz "mark_region"

.long mboot_ptr
.asciz "mboot_ptr"

.long mem_total
.asciz "mem_total"

.long mem_used
.asciz "mem_used"

.long memcpy
.asciz "memcpy"

.long memmove
.asciz "memmove"

.long memset
.asciz "memset"

.long messageLength
.asciz "messageLength"

.long mkdir_ext2
.asciz "mkdir_ext2"

.long mkdir_fs
.asciz "mkdir_fs"

.long mm_alloc_pages
.asciz "mm_alloc_pages"

.long mm_free_pages
.asciz "mm_free_pages"

.long module_direct_load
.asciz "module_direct_load"

.long module_load
.asciz "module_load"

.long modules_install
.asciz "modules_install"

.long mount_ext2
.asciz "mount_ext2"

.long node_from_file
.asciz "node_from_file"

.long now
.asciz "now"

.long open_ext2
.asciz "open_ext2"

.long open_fs
.asciz "open_fs"

.long open_pipe
.asciz "open_pipe"

.long outportb
.asciz "outportb"

.long outportl
.asciz "outportl"

.long outports
.asciz "outports"

.long outportsm
.asciz "outportsm"

.long page_fault
.asciz "page_fault"

.long paging_fini
.asciz "paging_fini"

.long pci_device_lookup
.asciz "pci_device_lookup"

.long pci_find_type
.asciz "pci_find_type"

.long pci_get_interrupt
.asciz "pci_get_interrupt"

.long pci_read_field
.asciz "pci_read_field"

.long pci_remap
.asciz "pci_remap"

.long pci_scan
.asciz "pci_scan"

.long pci_scan_bus
.asciz "pci_scan_bus"

.long pci_scan_func
.asciz "pci_scan_func"

.long pci_scan_hit
.asciz "pci_scan_hit"

.long pci_scan_slot
.asciz "pci_scan_slot"

.long pci_vendor_lookup
.asciz "pci_vendor_lookup"

.long pci_write_field
.asciz "pci_write_field"

.long pic_send_eoi
.asciz "pic_send_eoi"

.long pip_unsize
.asciz "pip_unsize"

.long pipe_available
.asciz "pipe_available"

.long pipe_check
.asciz "pipe_check"

.long pipe_increment_read
.asciz "pipe_increment_read"

.long pipe_increment_read_amount
.asciz "pipe_increment_read_amount"

.long pipe_increment_write
.asciz "pipe_increment_write"

.long pipe_increment_write_amount
.asciz "pipe_increment_write_amount"

.long pipe_size
.asciz "pipe_size"

.long pipe_unread
.asciz "pipe_unread"

.long plp
.asciz "plp"

.long prealloc_start
.asciz "prealloc_start"

.long printChar
.asciz "printChar"

.long prompt
.asciz "prompt"

.long qsort
.asciz "qsort"

.long ramdisk
.asciz "ramdisk"

.long ramdisk_mount
.asciz "ramdisk_mount"

.long read_block
.asciz "read_block"

.long read_cmos
.asciz "read_cmos"

.long read_eip
.asciz "read_eip"

.long read_ext2
.asciz "read_ext2"

.long read_fs
.asciz "read_fs"

.long read_inode
.asciz "read_inode"

.long read_pipe
.asciz "read_pipe"

.long readdir_ext2
.asciz "readdir_ext2"

.long readdir_fs
.asciz "readdir_fs"

.long readlink_ext2
.asciz "readlink_ext2"

.long readlink_fs
.asciz "readlink_fs"

.long realloc
.asciz "realloc"

.long refresh_inode
.asciz "refresh_inode"

.long reg
.asciz "reg"

.long relative_time
.asciz "relative_time"

.long reverse
.asciz "reverse"

.long rewrite_superblock
.asciz "rewrite_superblock"

.long scancodeToAscii
.asciz "scancodeToAscii"

.long scancodeToName
.asciz "scancodeToName"

.long schedule_no_irq
.asciz "schedule_no_irq"

.long screenBuffer
.asciz "screenBuffer"

.long scroll
.asciz "scroll"

.long secs_of_month
.asciz "secs_of_month"

.long secs_of_years
.asciz "secs_of_years"

.long selectcheck_fs
.asciz "selectcheck_fs"

.long selectwait_fs
.asciz "selectwait_fs"

.long serialLog
.asciz "serialLog"

.long serial_enable
.asciz "serial_enable"

.long serial_install
.asciz "serial_install"

.long serial_rcvd
.asciz "serial_rcvd"

.long serial_read_byte
.asciz "serial_read_byte"

.long serial_read_byte_async
.asciz "serial_read_byte_async"

.long serial_send_char
.asciz "serial_send_char"

.long serial_send_data
.asciz "serial_send_data"

.long serial_transmit_empty
.asciz "serial_transmit_empty"

.long setBackground
.asciz "setBackground"

.long setColor
.asciz "setColor"

.long setCursorLocation
.asciz "setCursorLocation"

.long setInterrupt
.asciz "setInterrupt"

.long setInterruptGate
.asciz "setInterruptGate"

.long setKernelStack
.asciz "setKernelStack"

.long setText
.asciz "setText"

.long set_block_number
.asciz "set_block_number"

.long set_frame
.asciz "set_frame"

.long set_nframes
.asciz "set_nframes"

.long setup_modules
.asciz "setup_modules"

.long shift
.asciz "shift"

.long snprintf
.asciz "snprintf"

.long split
.asciz "split"

.long start
.asciz "start"

.long startswith
.asciz "startswith"

.long strbackspace
.asciz "strbackspace"

.long strchr
.asciz "strchr"

.long strchrnul
.asciz "strchrnul"

.long strcmp
.asciz "strcmp"

.long strcpy
.asciz "strcpy"

.long strcpy_s
.asciz "strcpy_s"

.long strcspn
.asciz "strcspn"

.long strdup
.asciz "strdup"

.long strlen
.asciz "strlen"

.long strncmp
.asciz "strncmp"

.long strpbrk
.asciz "strpbrk"

.long strspn
.asciz "strspn"

.long strstr
.asciz "strstr"

.long strtok
.asciz "strtok"

.long strtok_r
.asciz "strtok_r"

.long switch_page_dir
.asciz "switch_page_dir"

.long symlink_ext2
.asciz "symlink_ext2"

.long symlink_fs
.asciz "symlink_fs"

.long syscall
.asciz "syscall"

.long syscallRoutine
.asciz "syscallRoutine"

.long test_frame
.asciz "test_frame"

.long timer_drift
.asciz "timer_drift"

.long timer_handler
.asciz "timer_handler"

.long timer_install
.asciz "timer_install"

.long timer_phase
.asciz "timer_phase"

.long timer_subticks
.asciz "timer_subticks"

.long timer_ticks
.asciz "timer_ticks"

.long tokenize
.asciz "tokenize"

.long tree_break_off
.asciz "tree_break_off"

.long tree_count_children
.asciz "tree_count_children"

.long tree_create
.asciz "tree_create"

.long tree_destroy
.asciz "tree_destroy"

.long tree_find
.asciz "tree_find"

.long tree_find_parent
.asciz "tree_find_parent"

.long tree_free
.asciz "tree_free"

.long tree_node_create
.asciz "tree_node_create"

.long tree_node_destroy
.asciz "tree_node_destroy"

.long tree_node_find
.asciz "tree_node_find"

.long tree_node_find_parent
.asciz "tree_node_find_parent"

.long tree_node_free
.asciz "tree_node_free"

.long tree_node_insert_child
.asciz "tree_node_insert_child"

.long tree_node_insert_child_node
.asciz "tree_node_insert_child_node"

.long tree_node_parent_remove
.asciz "tree_node_parent_remove"

.long tree_node_remove
.asciz "tree_node_remove"

.long tree_remove
.asciz "tree_remove"

.long tree_remove_reparent_root
.asciz "tree_remove_reparent_root"

.long tree_set_root
.asciz "tree_set_root"

.long tssEntry
.asciz "tssEntry"

.long tss_flush
.asciz "tss_flush"

.long unlink_ext2
.asciz "unlink_ext2"

.long unlink_fs
.asciz "unlink_fs"

.long vasprintf
.asciz "vasprintf"

.long vfs_install
.asciz "vfs_install"

.long vfs_mount
.asciz "vfs_mount"

.long vfs_mount_type
.asciz "vfs_mount_type"

.long vfs_register
.asciz "vfs_register"

.long virtual_to_physical
.asciz "virtual_to_physical"

.long write_block
.asciz "write_block"

.long write_ext2
.asciz "write_ext2"

.long write_fs
.asciz "write_fs"

.long write_inode
.asciz "write_inode"

.long write_inode_buffer
.asciz "write_inode_buffer"

.long write_pipe
.asciz "write_pipe"

.global kernel_symbols_end
kernel_symbols_end:
