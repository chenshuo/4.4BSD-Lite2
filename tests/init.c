int main()
{
  mbinit();

  int s = splimp();
  ifinit();
  domaininit();
  splx(s);
  return 0;
}
