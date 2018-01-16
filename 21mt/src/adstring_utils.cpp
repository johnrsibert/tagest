#include <fvar.hpp>
#include <adstring.hpp>

adstring& adstring_utils_tolower(adstring& ads);
adstring_array& adstring_utils_tolower(adstring_array& ads_array);
adstring& adstring_utils_toupper(adstring& ads);
adstring_array& adstring_utils_toupper(adstring_array& ads_array);
int adstring_utils_index(adstring_array& ads_array, const adstring& ads);

adstring& adstring_utils_tolower(adstring& ads)
{
  const int size = ads.size();
  for (int i = 1; i <= size; i++)
  {
    const int c = (int)ads(i);
    if (isalpha(c) && isupper(c))
      { ads(i) = (char)std::tolower(c); }
  }

  return ads;
}

adstring_array& adstring_utils_tolower(adstring_array& ads_array)
{
  const int size = ads_array.size();
  for (int i = 1; i <= size; i++)
    { adstring_utils_tolower(ads_array(i)); }

  return ads_array;
}

adstring& adstring_utils_toupper(adstring& ads)
{
  const int size = ads.size();
  for (int i = 1; i <= size; i++)
  {
    const int c = (int)ads(i);
    if (isalpha(c) && islower(c))
      { ads(i) = (char)std::toupper(c); }
  }

  return ads;
}

adstring_array& adstring_utils_toupper(adstring_array& ads_array)
{
  const int size = ads_array.size();
  for (int i = 1; i <= size; i++)
    { adstring_utils_toupper(ads_array(i)); }

  return ads_array;
}

int adstring_utils_index(adstring_array& ads_array, const adstring& ads)
{
  const int size = ads_array.size();
  for (int i = 1; i <= size; i++)
  {
    if (ads_array(i) == ads)
      { return i; }
  }

  return 0;
}
