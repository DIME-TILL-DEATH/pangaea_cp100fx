#include "console_handlers.h"

#include "eepr.h"
#include "ff.h"

#include "gui/amt.h"

#include "../gui/gui_task.h"

#include "console_helpers.h"
#include "resonance_filter_handlers.h"
#include "gate_handlers.h"
#include "compressor_handlers.h"
#include "preamp_handlers.h"
#include "amp_handlers.h"
#include "ir_handlers.h"
#include "eq_handlers.h"
#include "phaser_handlers.h"
#include "flanger_handlers.h"
#include "chorus_handlers.h"
#include "delay_handlers.h"
#include "early_handlers.h"
#include "reverb_handlers.h"
#include "tremolo_handlers.h"


static void get_amtdev_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	char hex[3] = {0, 0, 0};
	i2hex(5, hex);
	msg_console("%s\n", hex);
}

static void get_amtver_command_handler(TReadLine* rl, TReadLine::const_symbol_type_ptr_t* args, const size_t count)
{
	msg_console("%s\n", amt_ver);
}

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
}*/

static void get_state_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	for ( size_t i = 0 ; i < 512 ; i++ )
	{
		char hex[3] = {0,0,0} ;
		i2hex(presetData[i], hex);
		msg_console("%s" , hex ) ;
	}
	msg_console("\n") ;
}


//------------------------------------------------------------------------------
void ConsoleSetCmdHandlers(TReadLine* rl)
{
  SetConsoleCmdDefaultHandlers(rl);
/*
  rl->AddCommandHandler("cc", current_cabinet_command_handler);
  rl->AddCommandHandler("ce", cabinet_enable_command_handler);
  */


	rl->AddCommandHandler("amtdev", get_amtdev_command_handler);
	rl->AddCommandHandler("amtver", get_amtver_command_handler);

	rl->AddCommandHandler("state",  get_state_command_handler);

	set_resonance_filter_handlers(rl);
	set_gate_handlers(rl);
	set_compressor_handlers(rl);
	set_preamp_handlers(rl);
	set_amp_handlers(rl);
	set_ir_handlers(rl);
	set_eq_handlers(rl);
	set_phaser_handlers(rl);
	set_flanger_handlers(rl);
	set_chorus_handlers(rl);
	set_delay_handlers(rl);
	set_early_handlers(rl);
	set_reverb_handlers(rl);
	set_tremolo_handlers(rl);
}

//------------------------------------------------------------------------------
