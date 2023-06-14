extern "C" void iniconsole(natb cc);
extern "C" natq readconsole(char* buff, natq quanti);
extern "C" void writeconsole(const char* buff, natq quanti);

extern "C" void readhd_n(void* vetti, natl primo, natb quanti);
extern "C" void writehd_n(const void* vetto, natl primo, natb quanti);
extern "C" void dmareadhd_n(void* vetti, natl primo, natb quanti);
extern "C" void dmawritehd_n(const void* vetto, natl primo, natb quanti);

extern "C" natq getiomeminfo();

// ( ESAME 2023-01-11
// solo per debug e test
extern "C" int bufcache_status(natl *blocks, natl *read_sect);
//   ESAME 2023-01-11 )
