int main()
{
  loopattach(1);
  mbinit();

  int s = splimp();
  ifinit();
  domaininit();
  route_init();
  splx(s);
  setloopback();

  ping();
  return 0;
}
