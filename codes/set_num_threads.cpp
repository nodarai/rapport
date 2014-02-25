#ifndef _OPENMP
int get_suggested_omp_num_threads() {
  return 1;
}
#endif

#if defined _OPENMP
int get_suggested_omp_num_threads() {

  int default_num_threads=1, suggested_num_threads=1;
  
  char* env_var_c;
  env_var_c = getenv ("OMP_NUM_THREADS");
  if(env_var_c) 
    {
      return atoi(env_var_c);
    }
  //    cout<<"OMP_NUM_THREADS is not defined"<<endl;
  
  //set number of threads for appropriate OS
  int avload, nbofproc=omp_get_num_procs();
  FILE *iff;
    
#if defined(__APPLE__) || defined(__MACH__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__)
  cout<<"is MAC/*BSD"<<endl;
  iff= popen("echo $(sysctl -n vm.loadavg|cut -d\" \" -f2)", "r");
  if (!iff)
    {
      return default_num_threads;
    }
      
#elif defined(__linux__) || defined(__gnu_linux__) || defined(linux)
  iff= popen("cat /proc/loadavg |cut -d\" \" -f2", "r");
  if (!iff)
    {
      return default_num_threads;
    }
   
#elif defined (__unix) || (__unix__)
  iff=freopen("/proc/loadavg","r",stderr);
  fclose(stderr);
  if(!iff)
    {
      cout<<"your OS is not supported"<<endl;
      return default_num_threads;
    }
  iff= popen("cat /proc/loadavg 2>/dev/null|cut -d\" \" -f2", "r");
  if (!iff)
    {
      return default_num_threads;
    }

#elif defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
  iff= popen("wmic cpu get loadpercentage|more +1", "r");
  if (!iff)
    {
      return default_num_threads;
    }
  char buffer[4];
  char* c;
  c=fgets(buffer, sizeof(buffer), iff);
  if(!c)
    {
      return default_num_threads;
    }
  pclose(iff);
  int cout=0;
  while(buffer[count]!='\0' && buffer[count]!=' ')count++;
  for(int i=1,j=1;i<=count;i++,j*=10)
    avload+=(buffer[count-i]-'0')*j;
  suggested_num_threads=nbofproc-(int)(avload*((float)nbofproc/100)+0.5);
  return suggested_num_threads;

#else 
  cout<<"Can't define your OS"<<endl;
  return default_num_threads;
#endif

  char buffer[4];
  char* c;
  c=fgets(buffer, sizeof(buffer), iff);
  if(!c)
    {
      return default_num_threads;
    }
  pclose(iff);
  avload=(buffer[0]-'0')+((buffer[2]-'0')>5?1:0);

  suggested_num_threads=nbofproc-avload;
  return suggested_num_threads;
}
#endif
