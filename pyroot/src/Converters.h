// @(#)root/pyroot:$Name:  $:$Id: Converters.h,v 1.15 2005/11/24 16:25:18 pcanal Exp $
// Author: Wim Lavrijsen, Jan 2005
#ifndef PYROOT_CONVERTERS_H
#define PYROOT_CONVERTERS_H

// ROOT
#include "DllImport.h"
#include "TString.h"
#include "TClassRef.h"

// CINT
class G__CallFunc;
class G__TypeInfo;

// Standard
#include <string>
#include <map>


namespace PyROOT {

/** Python to CINT call converters
      @author  WLAV
      @date    01/26/2005
      @version 1.0
*/

   class TConverter {
   public:
      virtual ~TConverter() {}

   public:
      virtual Bool_t SetArg( PyObject*, G__CallFunc* ) = 0;
      virtual PyObject* FromMemory( void* address );
      virtual Bool_t ToMemory( PyObject* value, void* address );
   };

#define PYROOT_DECLARE_BASIC_CONVERTER( name )                                \
   class T##name##Converter : public TConverter {                             \
   public:                                                                    \
      virtual Bool_t SetArg( PyObject*, G__CallFunc* );                       \
      virtual PyObject* FromMemory( void* );                                  \
      virtual Bool_t ToMemory( PyObject*, void* );                            \
   }

#define PYROOT_DECLARE_BASIC_CONVERTER2( name, base )                         \
   class T##name##Converter : public T##base##Converter {                     \
   public:                                                                    \
      virtual PyObject* FromMemory( void* );                                  \
      virtual Bool_t ToMemory( PyObject*, void* );                            \
   }

#define PYROOT_DECLARE_ARRAY_CONVERTER( name )                                \
   class T##name##Converter : public TConverter {                             \
   public:                                                                    \
      T##name##Converter( int size = -1 ) { fSize = size; }                   \
      virtual Bool_t SetArg( PyObject*, G__CallFunc* );                       \
      virtual PyObject* FromMemory( void* );                                  \
      virtual Bool_t ToMemory( PyObject*, void* );                            \
   private:                                                                   \
     int fSize;                                                               \
   }

// converters for built-ins
   PYROOT_DECLARE_BASIC_CONVERTER( Long );
   PYROOT_DECLARE_BASIC_CONVERTER( LongRef );
   PYROOT_DECLARE_BASIC_CONVERTER( Bool );
   PYROOT_DECLARE_BASIC_CONVERTER( Char );
   PYROOT_DECLARE_BASIC_CONVERTER( UChar );
   PYROOT_DECLARE_BASIC_CONVERTER2( Short, Long );
   PYROOT_DECLARE_BASIC_CONVERTER2( UShort, Long );
   PYROOT_DECLARE_BASIC_CONVERTER2( Int, Long );
   PYROOT_DECLARE_BASIC_CONVERTER2( UInt, Long );
   PYROOT_DECLARE_BASIC_CONVERTER( ULong );
   PYROOT_DECLARE_BASIC_CONVERTER( LongLong );
   PYROOT_DECLARE_BASIC_CONVERTER( ULongLong );
   PYROOT_DECLARE_BASIC_CONVERTER( Double );
   PYROOT_DECLARE_BASIC_CONVERTER2( Float, Double );
   PYROOT_DECLARE_BASIC_CONVERTER( DoubleRef );

   class TVoidConverter : public TConverter {
   public:
      virtual Bool_t SetArg( PyObject*, G__CallFunc* );
   };

   class TMacroConverter : public TConverter {
   public:
      virtual Bool_t SetArg( PyObject*, G__CallFunc* );
      virtual PyObject* FromMemory( void* address );
   };

   class TCStringConverter : public TConverter {
   public:
      TCStringConverter( UInt_t maxSize = (UInt_t)-1 ) : fMaxSize( maxSize ) {}

   public:
      virtual Bool_t SetArg( PyObject*, G__CallFunc* );
      virtual PyObject* FromMemory( void* address );
      virtual Bool_t ToMemory( PyObject* value, void* address );

   private:
      std::string fBuffer;
      UInt_t fMaxSize;
   };

// pointer/array conversions
   class TVoidArrayConverter : public TConverter {
   public:
      TVoidArrayConverter( Bool_t keepControl = kTRUE ) { fKeepControl = keepControl; }
      virtual Bool_t SetArg( PyObject*, G__CallFunc* );
      virtual PyObject* FromMemory( void* address );
      virtual Bool_t ToMemory( PyObject* value, void* address );

   protected:
      virtual Bool_t GetAddressSpecialCase( PyObject* pyobject, void*& address );

   protected:
      Bool_t KeepControl() { return fKeepControl; }

   private:
      Bool_t fKeepControl;
   };

   PYROOT_DECLARE_ARRAY_CONVERTER( ShortArray );
   PYROOT_DECLARE_ARRAY_CONVERTER( UShortArray );
   PYROOT_DECLARE_ARRAY_CONVERTER( IntArray );
   PYROOT_DECLARE_ARRAY_CONVERTER( UIntArray );
   PYROOT_DECLARE_ARRAY_CONVERTER( LongArray );
   PYROOT_DECLARE_ARRAY_CONVERTER( ULongArray );
   PYROOT_DECLARE_ARRAY_CONVERTER( FloatArray );
   PYROOT_DECLARE_ARRAY_CONVERTER( DoubleArray );

   class TLongLongArrayConverter : public TVoidArrayConverter {
   public:
      virtual Bool_t SetArg( PyObject*, G__CallFunc* );
   };

// converters for special cases
#define PYROOT_DECLARE_STRING_CONVERTER( name, strtype )                      \
   class T##name##Converter : public TConverter {                             \
   public:                                                                    \
      virtual Bool_t SetArg( PyObject*, G__CallFunc* );                       \
      virtual PyObject* FromMemory( void* address );                          \
      virtual Bool_t ToMemory( PyObject* value, void* address );              \
   private:                                                                   \
      strtype fBuffer;                                                        \
   }

   PYROOT_DECLARE_STRING_CONVERTER( TString,   TString );
   PYROOT_DECLARE_STRING_CONVERTER( STLString, std::string );

   class TRootObjectConverter : public TVoidArrayConverter {
   public:
      TRootObjectConverter( const TClassRef& klass, Bool_t keepControl = kFALSE ) :
         TVoidArrayConverter( keepControl ), fClass( klass ) {}

   public:
      virtual Bool_t SetArg( PyObject*, G__CallFunc* );
      virtual PyObject* FromMemory( void* address );
      virtual Bool_t ToMemory( PyObject* value, void* address );

   protected:
      TClassRef fClass;
   };

   class TRefRootObjectConverter : public TRootObjectConverter {
   public:
      TRefRootObjectConverter( const TClassRef& klass, Bool_t keepControl = kFALSE ) :
         TRootObjectConverter( klass, keepControl ) {}

   protected:
      virtual Bool_t GetAddressSpecialCase( PyObject*, void*& ) { return kFALSE; }
   };

   class TRootObjectPtrConverter : public TRootObjectConverter {
   public:
      TRootObjectPtrConverter( const TClassRef& klass, Bool_t keepControl = kFALSE ) :
         TRootObjectConverter( klass, keepControl ) {}

   public:
      virtual Bool_t SetArg( PyObject*, G__CallFunc* );
      virtual PyObject* FromMemory( void* address );
      virtual Bool_t ToMemory( PyObject* value, void* address );
   };

   class TVoidPtrRefConverter : public TConverter {
   public:
      virtual Bool_t SetArg( PyObject*, G__CallFunc* );
   };

   class TVoidPtrPtrConverter : public TConverter {
   public:
      virtual Bool_t SetArg( PyObject*, G__CallFunc* );
   };

   PYROOT_DECLARE_BASIC_CONVERTER( PyObject );

// factories
   typedef TConverter* (*ConverterFactory_t) ( Long_t user );
   typedef std::map< std::string, ConverterFactory_t > ConvFactories_t;
   R__EXTERN ConvFactories_t gConvFactories;

// create converter from fully qualified type
   TConverter* CreateConverter( const std::string& fullType, Long_t user = -1 );

} // namespace PyROOT

#endif // !PYROOT_CONVERTERS_H
