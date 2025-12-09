# install <nolib.n>
# install <nolib.nb>
.n is nolang file
.nb is nolang book(libs)
half install <nolib.nb/nolib/nolib2>
put hogehoge()>{
  nolib.print("Hello");
  back 0;
}

put main()>{
  hogehoge();
}

===nolib.nb===
type nolib{
  put printlog(hogehoge);
  type nolib2{
    put hogehoge(hogehoge);
  }
}