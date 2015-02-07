plot.logistic<-function(s=1)
{
  y1=5
  y2=25
  t = 5
  x=seq(from=-t, to= 3*t, by = t/20)
  y = y1 + (y2-y1)*1.0/(1.0 + exp(-s*(x-t)))
  plot(x,y,type='l',lwd=3,ylim=c(0.9*y1,1.1*y2))
  abline(h=y1,lty="dotdash")
  abline(h=y2,lty="dotdash")
  abline(v=t,lty="dotdash")
  mtext(expression(y[2]),side=4,line=0.5,at=y2,las=1)
  mtext(expression(y[1]),side=4,line=0.5,at=y1,las=1)
  mtext(expression(x[i]),side=3,line=0.5,at=t)
}

plot.FAD.model<-function(t=5,y2=1)
{
  y1=0
  s = -1.0
  x=seq(from=0, to= 100, by = 1)
  y = y1 + (y2-y1)*1.0/(1.0 + exp(-s*(x-t)))
  plot(x,y,type='l',lwd=3,ylim=c(0.9*y1,1.1))
  abline(h=y1,lty="dotdash")
  abline(h=y2,lty="dotdash")
  abline(v=t,lty="dotdash")
  mtext(expression(y[2]),side=4,line=0.5,at=y2,las=1)
  mtext(expression(y[1]),side=4,line=0.5,at=y1,las=1)
  mtext(expression(x[i]),side=3,line=0.5,at=t)
}

plot.alt.logistic<-function(xA=-5,yA=-1,xB=5,yB=1,s=0.75)
{
  t = 0.5*(xA+xB)
  print(paste("t=",t))
# s = 0.75
  A = 1.0/(1.0 + exp(-s*(xA-t)))
  B = 1.0/(1.0 + exp(-s*(xB-t)))
  print(paste("A =",A,"B =",B))
  L = (A*yB - B*yA)/(A*(1.0-B)-B*(1.0-A))
  U = ((1.0-A)*yB-(1.0-B)*yA)/((1.0-A)*B-(1.0-B)*A)
  print(paste("L =",L,"U =",U))
  print(paste("yA =",yA,"yB =",yB))
  print(paste("xA =",xA,"xB =",xB))
  x=seq(from=xA,to=xB,by=0.05)
# print(x)
  y = L + (U-L)/(1.0 + exp(-s*(x-t)))
  plot(x,y,type='l',lwd=3)#,ylim=c(0.9*L,1.1*U))

  abline(h=U,lty="dotdash",col="red")
  mtext("U",side=4,line=0.5,at=U,las=1,col="red")

  abline(h=L,lty="dotdash",col="red")
  mtext("L",side=4,line=0.5,at=L,las=1,col="red")

  abline(v=t,lty="dotdash")
  mtext("t",side=1,line=0.5,at=t)

  abline(h=yA,col="blue")
  mtext(expression(y[A]),side=2,line=0.5,at=yA,las=1,col="blue")

  abline(h=yB,col="blue")
  mtext(expression(y[B]),side=2,line=0.5,at=yB,las=1,col="blue")

  abline(v=xA,col="blue")
  mtext(expression(x[A]),side=1,line=0.5,at=xA,las=1,col="blue")

  abline(v=xB,col="blue")
  mtext(expression(x[B]),side=1,line=0.5,at=xB,las=1,col="blue")

  dev.copy2pdf(file="alt-logistic.pdf")
}


plot.alt.FAD.model<-function(Fmax=100,Dmin=100,Dmax=1000,s=-0.075)
{
  yA = Dmin
  xA = Fmax
  yB = Dmax
  xB = 0.0

  t = 0.5*xA
  print(paste("t=",t))

  A = 1.0/(1.0 + exp(-s*(xA-t)))
  B = 1.0/(1.0 + exp(-s*(xB-t)))
  print(paste("A =",A,"B =",B))
  L = (A*yB - B*yA)/(A*(1.0-B)-B*(1.0-A))
  U = ((1.0-A)*yB-(1.0-B)*yA)/((1.0-A)*B-(1.0-B)*A)
  print(paste("L =",L,"U =",U))
  print(paste("yA =",yA,"yB =",yB))
  print(paste("Dmin=",Dmin,"Dmax=",Dmax))
  print(paste("xA =",xA,"xB =",xB))
  print(paste("s =",s,", t=",t))
  x=seq(from=xB,to=xA,by=0.05)
  y = L + (U-L)/(1.0 + exp(-s*(x-t)))

  plot(x,y,type='l',lwd=3,ylim=c(0.9*L,1.1*U))

  abline(h=U,lty="dotdash",col="red")
  mtext("U",side=4,line=0.5,at=U,las=1,col="red")

  abline(h=L,lty="dotdash",col="red")
  mtext("L",side=4,line=0.5,at=L,las=1,col="red")

  abline(v=t,lty="dotdash")
  mtext("t",side=1,line=0.5,at=t)

  abline(h=Dmin,col="blue")
  mtext(expression(D[min]),side=2,line=0.5,at=Dmin,las=1,col="blue")

  abline(h=Dmax,col="blue")
  mtext(expression(D[max]),side=2,line=0.5,at=Dmax,las=1,col="blue")

  abline(v=Fmax,col="blue")
  mtext(expression(F[max]),side=3,line=0.5,at=Fmax,las=1,col="blue")

  abline(v=xB,col="blue")
  mtext(expression(x[B]),side=3,line=0.5,at=xB,las=1,col="blue")

  dev.copy2pdf(file="alt-FADmodel.pdf")
}


