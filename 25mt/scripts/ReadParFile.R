# reads a post 2005 #v20r movemod par file
# single argument is a character string containing the file name
# returns a list containing all of the fields in a par file
# with names similar to those used in the par file comments
#
ReadParFile<-function(file)
{
  p <- scan(file,comment.char="#",what="character")
  
  M <- as.integer(p[1])
  N <- as.integer(p[2])
  deltax <- as.numeric(p[3])
  deltay <- as.numeric(p[4])
  SWLong <- p[5]
  SWLat <- p[6]
  GridMap <- matrix(as.integer(p[7:(M*N+6)]),nrow=M, ncol=N)
  nextp <- M*N+7
  boundary <- as.vector(as.integer(p[nextp:(nextp+3)]))
  nextp <- nextp+4
  nipar <-100
  ipar <- as.vector(as.integer(p[nextp:(nextp+nipar-1)]))
  nextp <- nextp + nipar

  nmonth <- as.integer(p[nextp])
  StartYear <- as.integer(p[nextp+1])
  StartMonth <- as.integer(p[nextp+2])
  nrelease <- as.integer(p[nextp+3])
  nfleet <- as.integer(p[nextp+4])
  NaturalMortality <- as.numeric(p[nextp+5])
  nextp <- nextp + 6
  SpecialMortality <- NA
  if (ipar[19] > 0)
  {
    SpecialMortality <-as.numeric(p[nextp])
    nextp <- nextp + 1
  } 
  
  ff<-matrix(p[nextp:(nextp+4*nfleet -1)],ncol=4,byrow=T)
  colnames(ff) <- c("fleet","q","R","DOK")
  ff<-as.data.frame(ff)
  nextp <- nextp + 4*nfleet

  tr<-matrix(as.numeric(p[nextp:(nextp+6*nrelease-1)]),ncol=6,byrow=T)
  colnames(tr) <- c("cohort","year","month","i","j","number")
  nextp <- nextp + 6*nrelease

  minsig <- as.numeric(p[nextp])
  maxsig <- as.numeric(p[nextp+1])
  minmort <- as.numeric(p[nextp+2])
  maxmort <- as.numeric(p[nextp+3])
  minq <- as.numeric(p[nextp+4])
  maxq <- as.numeric(p[nextp+5])
  MinRR <- as.numeric(p[nextp+6])
  MaxRR <- as.numeric(p[nextp+7])
  minvel <- as.numeric(p[nextp+8])
  maxvel <- as.numeric(p[nextp+9])
  nextp <- nextp + 10;

  NbPars <- NA
  if ( (ipar[11] == 24) || (ipar[11] == 25) )
  {
    NbPars <- as.vector(as.numeric(p[nextp:(nextp+nfleet-1)]))
    nextp <- nextp + nfleet
  }

  ngrid <- as.integer(p[nextp])
  nseason <- as.integer(p[nextp+1])
  nextp <- nextp + 2

  StartingMonth<-vector(mode = "logical", length = nseason)
  k <- matrix(nrow=ngrid,ncol=nseason)
  active <- matrix(nrow=ngrid,ncol=nseason)
  u <- matrix(nrow=ngrid,ncol=nseason)
  v <- matrix(nrow=ngrid,ncol=nseason)
  D <- matrix(nrow=ngrid,ncol=nseason)
  for (s in 1:nseason)
  {
    StartingMonth[s] <- as.integer(p[nextp])
    nextp <- nextp + 1 
  # print(paste(p[nextp],p[nextp+4]))
    for (r in 1:ngrid)
    {
    #  print(paste(r,s))
       k[r,s] <- as.integer(p[nextp])
       nextp <- nextp + 1 
       active[r,s] <- as.integer(p[nextp])
       nextp <- nextp + 1 
       u[r,s] <- as.numeric(p[nextp])
       nextp <- nextp + 1 
       v[r,s] <- as.numeric(p[nextp])
       nextp <- nextp + 1 
       D[r,s] <- as.numeric(p[nextp])
       nextp <- nextp + 1 
    }
  }
  npar<-NA
  if (nextp <= length(p))
  {
    npar<-as.integer(p[nextp])
    nextp <- nextp +1
  }
  L<-NA
  if (nextp <= length(p))
  {
    L<-as.numeric(p[nextp])
    nextp <- nextp +1
  }
  GMax<-NA
  if (nextp <= length(p))
  {
    GMax<-as.numeric(p[nextp])
    nextp <- nextp +1
  }

  return(list(M=M,N=N,deltax=deltax,deltay=deltay,SWLong=SWLong,SWLat=SWLat,
              GridMap=t(GridMap),boundary=boundary,
              ControlFlags=ipar,StartYear=StartYear,StartMonth=StartMonth,nrelease=nrelease,
              nfleet=nfleet,NaturalMortality=NaturalMortality, SpecialMortality=SpecialMortality, 
              FishingMortality=ff,tr=tr,
              minsig=minsig,maxsig=maxsig,minmort=minmort,maxmort=maxmort,
              minq=minq,maxq=maxq,MinRR=MinRR,MaxRR=MaxRR,minvel=minvel,maxvel=maxvel,
              NbPars=NbPars,ngrid=ngrid,nseason=nseason,StartingMonth=StartingMonth,
              k=k,active=active,u=u,v=v,D=D,
              npar=npar,L=L,GMax=GMax,

              nfield=nextp,lastp=length(p)))
}

getNaturalMortality<-function(fit)
{
  return(fit$NaturalMortality)
}

getFishingMortality<-function(fit)
{
  return(as.numeric(as.vector(fit$FishingMortality$q)))
}

do.it.all<-function()
{
  fits<-lapply(fit.files,ReadParFile)
  true<-ReadParFile(true.file)
  init<-ReadParFile(init.file)

  NaturalMortality<-unlist(lapply(fits,getNaturalMortality))
  boxplot(NaturalMortality)
  title("Natural Mortality")
  abline(h=true$NaturalMortality,col="blue")
  abline(h=init$NaturalMortality,col="red")

  FishingMortality.list<-lapply(fits,getFishingMortality)
  FishingMortality<-as.data.frame(matrix(unlist(FishingMortality.list),ncol=5,byrow=TRUE))
  colnames(FishingMortality)<-true$FishingMortality$fleet
  x11()
  boxplot(FishingMortality)
  title("Fishing Mortality by Fleet")
  for (i in 1:true$nfleet) points(i,as.numeric(as.vector(true$FishingMortality$q))[i],col="blue",pch=19,cex=1.2)
  for (i in 1:init$nfleet) points(i,as.numeric(as.vector(init$FishingMortality$q))[i],col="red",pch=19,cex=1.2)
}
# fit.files.34 <- as.list(c("b1.p01","b1.p02","b1.p03","b1.p04","b1.p05","b1.p06","b1.p07","b1.p08","b1.p09","b1.p0a","b1.p0b","b1.p0c","b1.p0d","b1.p0e","b1.p0f","b1.p10","b1.p11","b1.p12","b1.p13","b1.p14","b1.p15","b1.p16","b1.p17","b1.p18","b1.p19","b1.p1a","b1.p1b","b1.p1c","b1.p1d","b1.p1e","b1.p1f","b1.p20","b1.p21","b1.p22"))

init.file<-"b1.p00"
true.file<-"b1.q00"

fit.files<-as.list(c("b1.p01","b1.p02","b1.p03","b1.p04","b1.p05","b1.p06","b1.p07","b1.p08","b1.p09","b1.p0a","b1.p0b","b1.p0c","b1.p0d","b1.p0e","b1.p0f","b1.p10","b1.p11","b1.p12","b1.p13","b1.p14","b1.p15","b1.p16","b1.p17","b1.p18","b1.p19","b1.p1a","b1.p1b","b1.p1c","b1.p1d","b1.p1e","b1.p1f","b1.p20","b1.p21","b1.p22","b1.p23","b1.p24","b1.p25","b1.p26","b1.p27","b1.p28","b1.p29","b1.p2a","b1.p2b","b1.p2c","b1.p2d","b1.p2e","b1.p2f","b1.p30","b1.p31","b1.p32","b1.p33","b1.p34","b1.p35","b1.p36","b1.p37","b1.p38","b1.p39","b1.p3a","b1.p3b","b1.p3c","b1.p3d","b1.p3e","b1.p3f","b1.p40","b1.p41","b1.p42","b1.p43","b1.p44","b1.p45","b1.p46","b1.p47","b1.p48","b1.p49","b1.p4a","b1.p4b","b1.p4c","b1.p4d","b1.p4e","b1.p4f","b1.p50","b1.p51","b1.p52","b1.p53","b1.p54","b1.p55","b1.p56","b1.p57","b1.p58","b1.p59","b1.p5a","b1.p5b","b1.p5c","b1.p5d","b1.p5e","b1.p5f","b1.p60","b1.p61","b1.p62","b1.p63","b1.p64"))
