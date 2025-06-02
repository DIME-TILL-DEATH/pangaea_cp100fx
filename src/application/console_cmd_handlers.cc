#include "console_handlers.h"

#include "eepr.h"
#include "gu.h"
#include "math.h"
#include "ff.h"

#include "fonts/amt.h"

/*
static void current_cabinet_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	std::emb_string err_str ;
	if ( count == 1 )
	  {

	     console_out_currnt_cab(err_str, rl);
	     return ;
	  }
	if ( count == 3  )
	  {
	   if ( args[2][0] == '0' && strlen(args[2])== 1 )
		{
		   delete_current_cab(err_str, rl);
		   FATFS fs;
		   FRESULT res ;
		   FIL f;
	       UINT f_size;
	       res = f_mount ( &fs , "0:",  1);
	       if ( res != FR_OK )
	         {
    	       msg_console("error: 'f_mount' result '%s'\n", f_err2str(res) ) ;
    	       return ;
	         }
		   emb_string file_name;
		   file_name = "0:/Bank_";
		   file_name += (size_t)bank_pres[0];
		   file_name += "/Preset_";
		   file_name += (size_t)bank_pres[1];
		   file_name += "/" ;
		   file_name += args[1] ;

           FRESULT fs_res = f_open(&f, file_name.c_str(),  FA_CREATE_ALWAYS | FA_WRITE );

           if ( fs_res == FR_OK )
           {
        	   size_t chunk = 2 ;
        	   char hex[3] = {0,0,0} ;
        	   i2hex(chunk,hex);
        	   msg_console("RX %s\n",hex) ;
        	   do
        	     {
        	       int c ;
        	       char w   ;
		           for (size_t i = 0 ; i < chunk ; i++ )
		              {
                          rl->RecvChar(c);
		    	          if ( c == '\r')
		    	            {
		    	    	      f_close(&f);
		    	    	      f_mount(0, "0:", 0);
		    	    	      msg_console("END\n") ;
		    	    	      return ;
		    	            }
		    	          if(c > 57)c -= 39;
    	    	          w =  (c - '0') << 4 ;
		    	          rl->RecvChar(c);
  		    	          if ( c == '\r')
		    	            {
		    	          	  f_close(&f);
		    	          	  f_mount(0, "0:", 0);
		    	          	  msg_console("SYNC ERROR\n") ;
		    	          	  return ;
		    	            }
  		    	          if(c > 57)c -= 39;
  		    	          w  |=  c - '0' ;
		    	          UINT bw ;
		    	          FRESULT fs_res = f_write(&f, &w , 1 , &bw);
		    	          if ( bw == 0 || fs_res != FR_OK )
		    	            {
		    	    	      f_close(&f);
		    	    	      f_unlink(file_name.c_str());
		    	    	      msg_console("file write operation faild\n" ) ;
		    	    	      f_mount(0, "0:", 0);
		    	    	      return ;
		    	            }
		               }
		           msg_console("NX\n") ;
        	      }
		       while (1) ;
           }
           else
           {
        	   msg_console("error: file %s not created, 'f_open' result '%s'\n", args[1] , f_err2str(res)) ;
        	   f_mount(0, "0:", 0);
        	   return ;
           }
       }
	   else
	   {
		   // write to memory
		   //--------------------
		   return ;
	   }
	}
  msg_console("invalid args count\n" ) ;
}

static void get_state_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	for ( size_t i = 0 ; i < pdCount ; i++ )
	{
		char hex[3] = {0,0,0} ;
		i2hex(prog_data[i],hex);
		msg_console("%s" , hex ) ;
	}
	msg_console("\n") ;
}

static void cabinet_enable_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		char hex[3] = {0,0,0} ;
		i2hex( prog_data[cab_on],hex);
		msg_console("%s" , hex ) ;
		return ;
	 }
	 char* end ;
	 uint32_t val = strtol ( args[1] , &end, 16 );
	 prog_data[cab_on] = val ;
}
static void master_volume_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 char hex[3] = {0,0,0} ;
		 i2hex( prog_data[pres_lev],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = strtol ( args[1] , &end, 16 );
     prog_data[pres_lev] = val ;
     if(prog_data[cab_on])
        {
           sig_volume(powf(prog_data[pres_lev],2.0)*(1.0/powf(31.0,2.0)));
           p_vol = 1.0;
        }
     else p_vol = powf(prog_data[pres_lev],2.0)*(1.0/powf(31.0,2.0));
}
static void early_on_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 char hex[3] = {0,0,0} ;
		 i2hex( prog_data[er_on],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = strtol ( args[1] , &end, 16 );
     prog_data[er_on] = val ;
}
static void early_volume_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 char hex[3] = {0,0,0} ;
		 i2hex( prog_data[e_vol],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = strtol ( args[1] , &end, 16 );
     prog_data[e_vol] = val ;
     ear_vol = prog_data[e_vol]*(1.0/31.0);
}
static void early_type_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 char hex[3] = {0,0,0} ;
		 i2hex( prog_data[e_t],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = strtol ( args[1] , &end, 16 );
     prog_data[e_t] = val ;
}
static void presence_on_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 char hex[3] = {0,0,0} ;
		 i2hex( prog_data[pr_on],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = strtol ( args[1] , &end, 16 );
     prog_data[pr_on] = val ;
}
static void presence_volume_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 char hex[3] = {0,0,0} ;
		 i2hex( prog_data[pr_vol],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = strtol ( args[1] , &end, 16 );
     prog_data[pr_vol] = val ;
     set_shelf(prog_data[pr_vol]*(25.0/31.0));
}
static void eq_on_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	 if ( count == 1 )
	 {
		 char hex[3] = {0,0,0} ;
		 i2hex( prog_data[eq_on],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 char* end ;
     uint32_t val = strtol ( args[1] , &end, 16 );
     prog_data[eq_on] = val ;
}
static void eq_volume_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count)
{
	 uint8_t a = args[1][0] - 48;
	 if ( count == 2 )
	 {
		 char hex[3] = {0,0,0} ;
		 i2hex( prog_data[a],hex);
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 if ( count == 3  )
	 {
		 char* end ;
	     uint32_t val = strtol ( args[2] , &end, 16 );
	     prog_data[a] = val ;
	     filt_ini(a , prog_data + fr1 , prog_data + q1);
	     set_filt(a,prog_data[a]);
	 }
}
static void eq_frec_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count)
{
	 uint8_t a = args[1][0] - 48;
	 uint8_t aa = args[1][0] - 48 + fr1;
	 int8_t b = prog_data[a];
	 uint8_t c = abs(b);
	 if ( count == 2 )
	 {
		 char hex[4] = {0,0,0,0} ;
		 i2hex( c ,hex + 1);
		 if(b < 0)hex[0] = '-';
		 else hex[0] = ' ';
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 if ( count == 3  )
	 {
		 char* end ;
		 int32_t val = strtol ( args[2] , &end, 16 );
	     prog_data[aa] = val ;
	     filt_ini(a , prog_data + fr1 , prog_data + q1);
	     set_filt(a,prog_data[a]);
	 }
}
static void eq_q_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count)
{
	 uint8_t a = args[1][0] - 48;
	 uint8_t aa = args[1][0] - 48 + fr1;
	 int8_t b = prog_data[a];
	 uint8_t c = abs(b);
	 if ( count == 2 )
	 {
		 char hex[4] = {0,0,0,0} ;
		 i2hex( c ,hex + 1);
		 if(b < 0)hex[0] = '-';
		 else hex[0] = ' ';
		 msg_console("%s\n" , hex ) ;
		 return ;
	 }
	 if ( count == 3  )
	 {
		 char* end ;
		 int32_t val = strtol ( args[2] , &end, 16 );
	     prog_data[aa] = val ;
	     filt_ini(a , prog_data + fr1 , prog_data + q1);
	     set_filt(a,prog_data[a]);
	 }
}*/

static void get_amtname_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	msg_console("%s\n" , "cp100fx" ) ;
	msg_console("END\n") ;
}

static void get_amtver_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	msg_console("%s\n" , amt_ver ) ;
	msg_console("END\n") ;
}

//------------------------------------------------------------------------------
void ConsoleSetCmdHandlers(TReadLine* rl)
{
  SetConsoleCmdDefaultHandlers(rl);
/*
  rl->AddCommandHandler("cc", current_cabinet_command_handler);
  rl->AddCommandHandler("ce", cabinet_enable_command_handler);
  rl->AddCommandHandler("mv", master_volume_command_handler);
  rl->AddCommandHandler("gs", get_state_command_handler);
  rl->AddCommandHandler("eo", early_on_command_handler);
  rl->AddCommandHandler("ev", early_volume_command_handler);
  rl->AddCommandHandler("et", early_type_command_handler);
  rl->AddCommandHandler("po", presence_on_command_handler);
  rl->AddCommandHandler("pv", presence_volume_command_handler);
  rl->AddCommandHandler("eqo", eq_on_command_handler);
  rl->AddCommandHandler("eqv", eq_volume_command_handler);
  rl->AddCommandHandler("eqf", eq_frec_command_handler);
  rl->AddCommandHandler("eqq", eq_q_command_handler);*/


  rl->AddCommandHandler("amtname", get_amtname_command_handler);
  rl->AddCommandHandler("amtver",  get_amtver_command_handler);
}

//------------------------------------------------------------------------------
