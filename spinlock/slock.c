unsigned int mutex=1;
void inline enter_critical(void)
{
  asm("spin_loop: lock btr $0,mutex");
  asm("           jnc spin_loop");
}

void inline leave_critical(void)
{
  asm("        bts $0,mutex");
}
