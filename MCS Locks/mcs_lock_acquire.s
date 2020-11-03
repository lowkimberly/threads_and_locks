	.file	"mcs_lock_aquire.c"
	.text
.globl aquire_lock
	.type	aquire_lock, @function
	
############################################
aquire_lock:
############################################
	pushl %ebp
	movl %esp, %ebp
	pushl %edi
	pushl %esi
	pushl %ebx
	subl $16, %esp
	
	movl 12(%ebp), %eax		#set eax as I (a pointer to a qnode)
	movl $0, (%eax)			#clear the next pointer for I

	movl 8(%ebp), %ecx		#set ecx as *lock (a pointer to lock)
	lock xchgl %eax, (%ecx)		#exchange addresses pointed to by *lock and *I
	cmpl $0, %eax			#Compare precedessor to NULL
	je .return			#Jump to return if there was no predecessor

	movl 12(%ebp), %ecx		#set ecx to I address
	movl %ecx, (%eax)		#set precessor->next to I
	movl 12(%ebp), %eax		#set eax as I - again again, not needed?
	movb $1, 4(%eax)		#set the locked variable for I to 1 (use movb because locked is a char and therefore a byte)

.Locked:
	movl 12(%ebp), %eax		#move I to eax - AGAIN
	movzbl 4(%eax), %eax		#move I.locked to eax
	testb %al, %al			#compare I.locked to 1
	jne .Locked			#if I.locked == 1 (it is still locked) loop again
	
.return:
	addl $16, %esp
	popl %ebx
	popl %esi
	popl %edi
	popl %ebp
	ret

