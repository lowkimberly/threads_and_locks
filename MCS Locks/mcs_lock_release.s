	.file	"mcs_lock_release.c"
	.text
.globl release_lock
	.type	release_lock, @function
############################################
release_lock:
############################################
	pushl %ebp
	movl %esp, %ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	subl $16, %esp

	movl 12(%ebp), %eax		#set eax as I (a pointer to a qnode)
	cmpl $0, (%eax)			#compare I.next to NULL
	je .swap			#if I.next is NULL go to swap
	jmp .release			#otherwise jump to release
	
.while_loop:
	movl 12(%ebp), %eax		#set eax as I (a pointer to a qnode)
	cmpl $0, (%eax)			#compare I.next to NULL
	jne .release			#if I.next is not NULL go to release

.swap:
	movl 8(%ebp), %ecx		#set %ecx as the address of lock
	movl 12(%ebp), %eax		#set %eax as the address of I
    lock cmpxchgl %eax, (%ecx)      #compare and exchange the addresses of lock and I
	jne .while_loop			#if lock and I are not equal return to the top of the loop
	movl 8(%ebp), %ecx		#set %ecs as the address of the lock pointer
	movl $0, (%ecx)			#set lock.next to NULL
	jmp .return			#return

.release:
	movl 12(%ebp), %eax		#set eax as I (a pointer to a qnode)
	movl (%eax), %eax		#move I.next to eax
	movb $0, 4(%eax)		#set I.next.locked to zero (use movb because locked is an unsigned byte)

.return:
	addl $16, %esp
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	ret
	
