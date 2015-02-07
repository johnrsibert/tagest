get.sst.from.server<-function(lonlow, lonhigh, latlow, lathigh, datelow, datehigh, folder=tempdir(), 
                              server='http://atlas.nmfs.hawaii.edu/cgi-bin/reynolds_extract.py'){
  require(date)
  testdir<-file.info(folder)$isdir
  if(is.na(testdir)){
    dir.create(folder)
  }else{
    if(!testdir)stop("The folder name supplied is in fact a filename")
  }
  
  # add a few days for good measure and
  # because data source is 8-day files
  minDate<-mdy.date(datelow[2],datelow[3],datelow[1])-10
  maxDate<-mdy.date(datehigh[2],datehigh[3],datehigh[1])+10

  yrRan<-c(date.mdy(minDate)$year,date.mdy(maxDate)$year)
  daysIntoYearRan<-c(minDate-mdy.date(1,1,yrRan[1]),maxDate-mdy.date(1,1,yrRan[2]))

  lonRan<-c(lonlow,lonhigh)
  latRan<-c(latlow,lathigh)

  string<-''
  string<-paste(string,'?lon1=',lonRan[1],'&lon2=',lonRan[2],sep='')
  string<-paste(string,'&lat1=',latRan[1],'&lat2=',latRan[2],sep='')
  string<-paste(string,'&year1=',yrRan[1],'&day1=',daysIntoYearRan[1],sep='')
  string<-paste(string,'&year2=',yrRan[2],'&day2=',daysIntoYearRan[2],sep='')
  link<-paste(server,string, sep='')
  dest<-paste(folder,'temp.zip', sep='/')
  download.file(link, dest, mode='wb')
  .Internal(int.unzip(paste(folder,'temp.zip',sep='/'),NULL, folder))
  # May need some special treatment for windows here using 'zip.unpack()'
  unlink(paste(folder,'temp.zip', sep='/'))
  .sstFileVector<<-paste(folder,dir(folder), sep='/')
  for(f in .sstFileVector){
    dat<-read.table(f, head=FALSE)
    write.table(dat[complete.cases(dat),], col.names=FALSE, row.names=FALSE, quote=FALSE, file=f)
  }
  return(folder)
}  

getMonths<-function(path, file=NULL){
  filenames<-dir(path)
  from.yr<-as.integer(substr(filenames,3,6))
  from.day<-as.integer(substr(filenames,7,9))
  to.yr<-as.integer(substr(filenames,11,14))
  to.day<-as.integer(substr(filenames,15,17))
  middate<-as.date(0.5*((mdy.date(month=1,day=1,year=from.yr)+from.day-1)+
                        (mdy.date(month=1,day=1,year=to.yr)+to.day-1)))
  mdy<-date.mdy(middate)	
  yrmonth<-paste(mdy$year, month.abb[mdy$month], sep='')
  getMonth<-function(str){
    fn<-filenames[which(yrmonth==str)]
    datalist<-lapply(paste(path, fn, sep='/'), read.table, head=FALSE)
    combdata<-datalist[[1]]
    for(i in 2:length(datalist)){
      combdata<-rbind(combdata, datalist[[i]])
    }
    ret<-aggregate(combdata[,3], list(lat=combdata[,1],lon=combdata[,2]), mean, na.rm=TRUE)
    ret<-cbind(substr(str,1,4), substr(str,5,7), ret[,c(2,1,3)])
    names(ret)<-c('year','month','lon','lat','sst')
    return(ret)
  }
  ym<-unique(yrmonth)
  monthList<-lapply(ym,getMonth)
  if(!is.null(file)){
    write.table(monthList[[1]], file=file, row.names=FALSE, quote=FALSE, sep='\t')
    for(i in 2:length(monthList)){
      write.table(monthList[[i]], file=file, col.names=FALSE, row.names=FALSE, 
                  quote=FALSE, sep='\t', append=TRUE)
    }
  }
  return(monthList)
}

# An example
#path<-get.sst.from.server(150,180,0,20,c(2003,1,13),c(2004,2,17))  
path<-get.sst.from.server(130,190,-25,10,c(1977,10,1),c(1982,9,1))  
sstlist<-getMonths(path) # saves to sstlist
sstlist<-getMonths(path, "grid10.sst2") # saves to sstlist and in file
