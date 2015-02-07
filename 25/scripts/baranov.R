# dvariable bound(const dvariable& x, const double& l, const double& h, const double& eps)
# {
#   dvariable ret;
#   if((x>=(l+eps))&&(x<=(h-eps))){
#     ret=x;
#   }else{
#     if(x<(l+eps)){
#       ret=eps*exp((x-(l+eps))/eps)+l;
#     }else{
#       if(x>(h-eps)){
#         ret=h-eps*exp(((h-eps)-x)/eps);
#       }
#     }
#   }
#   return ret;
# }

bound<-function(x, l, h, eps)
  {
    if((x>=(l+eps))&&(x<=(h-eps)))
    {
      ret=x
    }
    else
    {
      if(x<(l+eps))
      {
        ret=eps*exp((x-(l+eps))/eps)+l
      }
      else
      {
        if(x>(h-eps))
        {
          ret=h-eps*exp(((h-eps)-x)/eps)
        }
      }
    }
    return(ret)
  }

tbound<-function(x1,x2,l=0,h=10)
{
  x<-seq(x1,x2,0.001)
  y<-vector(length=length(x))
  for (i in 1:length(x))
    y[i]=bound(x[i],l,h,1e-3)

  prange<-c(0.998*h,1.002*h)
  plot(x,y,type='l',pch='+',xlim=prange,ylim=prange)
}


gF<-function(F,M)
{
  g<-(F/(F+M))*(1-exp(-F-M))
  return(g)
}
# g<-expression((F/(F+M))*(1-exp(-F-M)))
# D(g,"F")
#(1/(F + M) - F/(F + M)^2) * (1 - exp(-F - M)) + (F/(F + M)) * exp(-F - M)

dgdf<-function(F,M)
{
  d <- (1/(F + M) - F/(F + M)^2) * (1 - exp(-F - M)) + (F/(F + M)) * exp(-F - M)
  return(d)
}


BCE<-function(F,M,P)
{
  C <- gF(F,M)*P
  return(C)
}

NRF<-function(M,P,C,eps=1e-5)
{
  maxit <- 25
  it <- 0
  F <- C/P
  df <- 100*eps
  while ((df > eps) && (F < 5))
  {
    g <- C/BCE(F,M,P) - 1.0
    dg <- dgdf(F,M)*P
    df <- g/dg
    F <- F + df
    it <- it + 1
#   print(paste("iteration ", it, ", df = ", df,", F = ", F,sep=""))
    if (it > maxit)
      break;
  }
  return(F)
}

test_gdiff<-function(F=0.1,M=0.1, P=50,eps=1e-5)
{
  tF0 <- seq(0,2,.1)
  for (F in tF0)
  {
     C1<-BCE(F-eps,M,P)
     C2<-BCE(F+eps,M,P)
     ndcdf = (C2-C1)/(2.0*eps)
     adcdf = dgdf(F,M)*P
     print(paste(F,ndcdf,adcdf,(ndcdf-adcdf)))
  }
}

testNRF1<-function(F=0.05, M=0.1, P=50)
{
  C <- BCE(F, M, P)
  print(paste(P,C, F))
  estF0 <- NRF(M, P, C)
  return(estF0)
}

testNRF<-function(M=0.1,P=5.35,eps=1e-5)
{
  Cseq<-seq(1,10,1)
  for (C in Cseq)
  {
     print("")
     print(paste("**",C,P,sep=" "))
     estF <- NRF(M, P, C, eps)
     print(paste("  ",C,BCE(estF,M,P),estF,sep=" "))
  }
}

######################################################
CE<-function(F1,F0,M,P)
{
  z <- M + F1 +F0
  C<-F0/z*(1.0-exp(-z))*P
  return(C)
}

dce_df0<-function(F1,F0,M,P)
{
  z <- M + F1 +F0
#dCdF0 = -F0*exp(-z)*P/z - (1.0-exp(-z))*P/z + F0*(1-exp(-z))*P/(z*z)
#dCdF0 = (((1/(F1 + F0 + M) - F0/(F1 + F0 + M)^2) * (1 - exp(-F1 - F0 - M)) + F0/(F1 + F0 + M) * exp(-F1 - F0 - M)) * P)
 dCdF0 = (((1/z - F0/z^2) * (1 - exp(-z)) + F0/z * exp(-z)) * P)
  return(dCdF0)
}

#R: D
# -(((1/(F1 + F0 + M) - F0/(F1 + F0 + M)^2) * (1 - exp(-F1 - F0 - M)) + F0/(F1 + F0 + M) * exp(-F1 - F0 - M)) * P)

# maxima:
#(%i1) diff(C-F/(F+M)*(1-exp(-(F+M)))*P,F);
#                 - M - F            - M - F               - M - F
#             F %e        P   (1 - %e       ) P   F (1 - %e       ) P
#(%o1)      - ------------- - ----------------- + -------------------
#                 M + F             M + F                     2
#                                                      (M + F)


NR_F0<-function(F1,F0,M,P,C,it)
{
  maxit <- 25
  eps <- 1e-5
  dx <- 1.0
  it <- 0
  while (dx > eps)
  {
    g <- C-CE(F1,F0,M,P)
    dg <- dce_df0(F1,F0,M,P)
    dx <- g/dg
    F0 <- F0 + dx
    it <- it + 1
    print(paste("iteration ", it, ", dx = ", dx,", F0 = ", F0,sep=""))
    if (it > maxit)
      break;
  }
  return(F0)
}

testNR<-function(F1=0.1, F0=0.05, M=0.1, P=50)
{
  C <- CE(F1, F0, M, P)
  print(paste(P,C, F0))
  estF0 <- NR_F0(F1,0.0, M, P, C, it)
  return(estF0)

}

test_diff<-function(F1=0.1,M=0.1, P=50,eps=1e-5)
{
  tF0 <- seq(0,2,.1)
  for (F0 in tF0)
  {
     C1<-CE(F1,F0-eps,M,P)
     C2<-CE(F1,F0+eps,M,P)
     ndcdf = (C2-C1)/(2.0*eps)
     adcdf = dce_df0(F1,F0,M,P)
     print(paste(F0,ndcdf,adcdf,(ndcdf-adcdf)))
  }
}

DD<-function()
{

  g <- expression(C-F0/(F1+F0+M)*(1-exp(-F1-F0-M))*P)
  dg <- D(g,"F0")
  print(dg)
  
}
