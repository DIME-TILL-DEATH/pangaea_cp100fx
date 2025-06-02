/*
 * fs_browser.h
 *
 *  Created on: 31 марта 2014 г.
 *      Author: klen
 */

#ifndef __FS_BROWSER_H__
#define __FS_BROWSER_H__

#include "appdefs.h"
#include "ff.h"
#include <vector>
#include <algorithm>

using namespace std ;

typedef enum { fotFile, fotDir, fotUnknown } fs_object_type_t ;

typedef struct
{
  emb_string name ; // имя файла или каталога
  emb_string dir ; // родительская директория файла с уровнем вложенности в виде симвлов звездочек
  emb_string full_dir ;
  emb_string startup ;
  fs_object_type_t type ; // тип объекта
} fs_object_t ;



typedef  vector<fs_object_t> fs_object_list_t ;
typedef  void (*outstr_func_t) ( const emb_string& string ) ;
extern volatile uint8_t flash_buf[1536];

class TFsBrowser
{
  public:

    typedef enum { bcCurrent, bcUp, bcDown, bcAction, bcPwd, bcList, bcFsMount, bcFsUmount , bcLoadImp, bcStartup , bcBack} browse_command_t ;

    TFsBrowser() ;
    ~TFsBrowser()  ;

    void Browse(browse_command_t browse_command, fs_object_t& object, fs_object_list_t& object_list) ;
    bool GetCabConfig(uint8_t* cab_data, emb_string& err_msg) ;
    inline const fs_object_list_t::iterator CurrentObject() const { return curr_fs_object ; }
    void Print ( outstr_func_t func ) ;
    inline bool SD_Initilized() const {return sd_initialized ;} ;
    inline emb_string CurrDir () const { return curr_dir_level + curr_dir_name; }
    void Load_cab(fs_object_t& object);

  protected:


    void  PrevObject( fs_object_t& object ) ;
    void  NextObject( fs_object_t& object ) ;
    void  SelectDir ( fs_object_t object ) ;
    void  SelectFile (const fs_object_t& object ) ;
    inline const fs_object_list_t& List() const { return fs_object_list ; }  ;
    void CollapseAbsPath(char* abs_path , emb_string& name,  emb_string& level);
    FRESULT FsMount(const emb_string& init_dir );
    FRESULT FsUmount();


  private:

    fs_object_list_t fs_object_list ;
    fs_object_list_t::iterator curr_fs_object ;
    emb_string curr_dir_name ;
    emb_string curr_dir_level ;

    bool sd_initialized ;
    FATFS fs ;
    FRESULT fs_res ;

    struct sort_fs_object /*: public std::binary_function<fs_object_t, fs_object_t, bool>*/
    {
        inline bool operator()(const fs_object_t& a, const fs_object_t& b) const
        {
          if ( a.type == b.type )
            return a.name < b.name ;

          return a.type > b.type ;
        };
    };
} ;

#endif /* __FS_BROWSER_H__ */
