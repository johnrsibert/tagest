//$Id: node_flags.cpp 2754 2011-01-02 20:57:07Z jsibert $
  int knode = 0;
  if (year_flag)
  {
    knode ++;
    input_layer(knode) = scaled_year(ym.get_year());
  }
  if (month_flag)
  {
    knode ++;
    input_layer(knode) = scaled_month(ym.get_month_value());
  }
  for(int i = 1; i <= m; i++)
  {
    int inode = knode;
    if (latitude_flag)
    {
      inode ++;
      input_layer(inode) = scaled_latitude(i);
    }
    for(int j =1; j <= n;  j++)
    {
      int jnode = inode;
      if (longitude_flag)
      {
        jnode ++;
        input_layer(jnode) = scaled_longitude(j);
      }
      if (depth_flag)
      {
        jnode ++;
        input_layer(jnode) = scaled_depth(i,j);
      }
      if (elevation_flag)
      {
        jnode ++;
        input_layer(jnode) = scaled_elevation(i,j);
      }
      if (fad_flag)
      {
        jnode ++;
        input_layer(jnode) = scaled_numfads(i,j);
      }
      if (distance_flage)
      {
        jnode ++;
        input_layer(jnode) = scaled_distance(i,j);
      }
    } //for j
  } //for i
