inline void par_ind_num(uint8_t val)
{
  uint8_t prog_num[2];
  uint8_t a;
  if(val != 0xff){
  if(val < 32)
    {
      t12x13_clear(91,4,36);
      prog_num[0] = val / 10 + 48;
      if(prog_num[0] == 48)
        {
          prog_num[0]=32;
          a = 95;
        }
      else a = 97;
      prog_num[1] = val % 10 + 48;
      for (uint8_t i = 0 ;i < 2;i++)a += t12x13_sym(a,4,prog_num[i],0);
    }
  }
}
inline void par_ind_num_(uint8_t col , uint8_t pag , uint16_t val)
{
  uint8_t prog_num[3];
  uint8_t a;
  Arsys_clean_(col , pag , 3);
  prog_num[0] = val / 100;
  prog_num[1] = (val % 100) / 10;
  prog_num[2] = (val % 100) % 10;
  if(val > 99)a = 0;
  else {
	  if(val > 9)a = 1;
	  else a = 2;
  }
  col += (a * 3);
  for (uint8_t i = a ; i < 3 ; i++)col = Arsys_sym(col,pag,prog_num[i] + 48,0);
}
inline uint8_t ind_num_pan(uint8_t col , uint8_t pag , uint8_t val)
{
  uint8_t prog_num[2];
  uint8_t a;
  uint8_t b;
  if(val < 63)
	  {
	  	  b = 0;
	  	  val = 63 - val;
	  }
  else {
	  b = 1;
	  val = val - 63;
	  if(!val)b = 2;
  }
  Arsys_clean_(col , pag , 3);
  prog_num[0] = val / 10;
  prog_num[1] = val % 10;
  if(val < 10)a = 1;
  else a = 0;
  col += (a * 3 + 3);
  for (uint8_t i = a ; i < 2 ; i++)col = Arsys_sym(col,pag,prog_num[i] + 48,0);
  return b;
}
inline void par_ind_num_pan(uint8_t col , uint8_t pag , uint8_t val)
{
  switch(ind_num_pan(col,pag,val)){
  case 0:Arsys_sym(col - 40 , pag , 76 , 0);break;
  case 1:Arsys_sym(col - 40 , pag , 82 , 0);break;
  case 2:Arsys_sym(col - 40 , pag , 32 , 0);break;
  }
}
const uint8_t dry[] = "Dry";
const uint8_t wet[] = "Wet";
const uint8_t dw_[] = "D+W";
inline void par_ind_num_mix(uint8_t col , uint8_t pag , uint8_t val)
{
  switch(ind_num_pan(col,pag,val)){
  case 0:if(col < 98)Arsys_line(col - 54 , pag , (uint8_t*)dry , 0);break;
  case 1:if(col < 98)Arsys_line(col - 54 , pag , (uint8_t*)wet , 0);break;
  case 2:if(col < 98)Arsys_line(col - 54 , pag , (uint8_t*)dw_ , 0);break;
  }
}
inline uint8_t par_ind(uint8_t col , uint8_t pag , uint32_t val)
{
  Set_Column_Address(col);
  Set_Page_Address(pag);
  GPIO_ResetBits(GPIOB,CS);
  for(uint8_t i = 0 ; i < 33 ; i++)
    {
      if(((val >> 2) >= i) || (i == 0) || (i == 32))oled023_1_send_data(0x7e);
      else oled023_1_send_data(0x42);
    }
  GPIO_SetBits(GPIOB,CS);
  par_ind_num_(col + 34 , pag , val);
  return 0;
}
inline void p_ind(uint8_t col , uint8_t pag , uint32_t val)
{
  uint8_t val1 = val >> 2;
  Set_Column_Address(col);
  Set_Page_Address(pag);
  for(uint8_t i = 0 ; i < 32 ; i++)
    {
      if(((val1 + 1) == i) || (i == 0))oled023_1_write_data(0x7e);
      else oled023_1_write_data(0x42);
    }
  Set_Column_Address(col + 31);
  if((val1 + 1) == 32)oled023_1_write_data(0x7e);
  else Set_Column_Address(col + 32);
  oled023_1_write_data(0x7e);
}
inline void par_ind_pan(uint8_t col , uint8_t pag , uint32_t val)
{
  p_ind(col,pag,val);
  par_ind_num_pan(col + 34 , pag , val);
}
inline void par_ind_mix(uint8_t col , uint8_t pag , uint32_t val)
{
  p_ind(col,pag,val);
  par_ind_num_mix(col + 34 , pag , val);
}
