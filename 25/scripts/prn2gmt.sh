#!/usr/bin/gawk -f
BEGIN{
  M = 0
  N = 0
  deltax = 60
  deltay = 60
  sw_lon = "180E"
  sw_lat = "0N"
  x0 = 0
  y0 = 0
# print M,N,deltax,deltay,sw_lon,sw_lat,x0,y0
}

{
# print "NR,NF ",NR,NF,$0
  if (NR == 3)
  {
#   print $0
    M = $1
    N = $2
    deltax = $3
    deltay = $4
    dx = deltax/60
    dy = deltay/60
    sw_lon = $5
    sw_lat = $6
    x0 = lon_field(sw_lon)
    y0 = lat_field(sw_lat)
    # y = (N + y0)*dy
    y = y0 + N*dy
  # print M,N,deltax,deltay,sw_lon,sw_lat,x0,y0,dx,dy,y
  }
  if (NF == M)
  {
    y = y - dy
    yy = y + 0.5*dy
#   print "y,yy ",y,yy
    for (i = 1; i <= M; i++)
    {
      if ($i == 0)
      {
        xx = x0 + (i-1)*dx + 0.5*dx
        print xx,yy
      }

    }
  }
}

#END{
  #print M,N,deltax,deltay,sw_lon,sw_lat,x0,y0
#}

function lon_field(field)
{
  sub("E","",field)
  sub("W","",field)
  return(field)
}
 
function lat_field(field)
{
  tmp = field
  sub("S","",tmp)
  sub("N","",tmp)
  nc = length(field)
  if (substr(field,nc) == "S")
    tmp = -tmp
  return(tmp)
}
