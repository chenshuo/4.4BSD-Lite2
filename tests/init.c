int main()
{
  loopattach(1);
  mbinit();

  int s = splimp();
  ifinit();
  domaininit();
  splx(s);
  setloopback();
  return 0;
}
