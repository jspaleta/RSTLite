/******************************************************************************
*
*  NSSDC/CDF                    Run-length encoding compression/decompression.
*
*  Version 1.0, 15-May-96, Hughes STX.
*
*  Modification history:
*
*   V1.0  15-May-96, J Love     Original version.
*
******************************************************************************/

#include "cdflib.h"
#include "cdflib64.h"

#if !SUPPORT_RLE64 && defined(BORLANDC)
#pragma warn -par
#endif

/******************************************************************************
* CompressRLE0_64.
******************************************************************************/

STATICforIDL CDFstatus CompressRLE0_64 (srcFp, srcOffset, srcSize, srcError,
				        destFp, destOffset, destSize, destError)
vFILE *srcFp;
OFF_T srcOffset;
OFF_T srcSize;
CDFstatus srcError;
vFILE *destFp;
OFF_T destOffset;
OFF_T *destSize;
CDFstatus destError;
{
#if SUPPORT_RLE64
  CDFstatus pStatus = CDF_OK; Int32 byteN = 0, count;
  uByte byte, zero = 0, max255 = (uByte) 255, zCount;
  /****************************************************************************
  * Seek to starting offsets.
  ****************************************************************************/
  if (!SEEKv64(srcFp,srcOffset,vSEEK_SET)) return srcError;
  if (!SEEKv64(destFp,destOffset,vSEEK_SET)) return destError;
  /****************************************************************************
  * Initialize destination count and begin...
  ****************************************************************************/
  *destSize = 0;
  for (;;) {
     /*************************************************************************
     * Return if past the last source byte.
     *************************************************************************/
     if (byteN == srcSize) return pStatus;
     /*************************************************************************
     * Read the next source byte.
     *************************************************************************/
     if (!READv64(&byte,1,1,srcFp)) return srcError;
     byteN++;
     /*************************************************************************
     * If the source byte is zero, set the counter and begin...
     *************************************************************************/
     if (byte == 0) {
       count = 1;
       for (;;) {
	  /********************************************************************
	  * If past the last source byte, write a zero/count to the
	  * destination file and return.  In this case the final `count'
	  * bytes of the source file are zeroes.
	  ********************************************************************/
	  if (byteN == srcSize) {
	    if (!WRITEv64(&zero,1,1,destFp)) return destError;
	    (*destSize)++;
	    zCount = (uByte) (count - 1);
	    if (!WRITEv64(&zCount,1,1,destFp)) return destError;
	    (*destSize)++;
	    return pStatus;
	  }
	  /********************************************************************
	  * Read the next source byte.
	  ********************************************************************/
	  if (!READv64(&byte,1,1,srcFp)) return srcError;
	  byteN++;
	  /********************************************************************
	  * If the byte is not a zero, write a zero/count and the non-zero
	  * byte to the destination file and break out of this loop.
	  ********************************************************************/
	  if (byte != 0) {
	    if (!WRITEv64(&zero,1,1,destFp)) return destError;
	    (*destSize)++;
	    zCount = (Byte) (count - 1);
	    if (!WRITEv64(&zCount,1,1,destFp)) return destError;
	    (*destSize)++;
	    if (!WRITEv64(&byte,1,1,destFp)) return destError;
	    (*destSize)++;
	    break;
	  }
	  /********************************************************************
	  * Increment for another zero and check if at the limit.  If so,
	  * write a zero/count to the destination and break out of this loop.
	  ********************************************************************/
	  count++;
	  if (count == 256) {
	    if (!WRITEv64(&zero,1,1,destFp)) return destError;
	    (*destSize)++;
	    if (!WRITEv64(&max255,1,1,destFp)) return destError;
	    (*destSize)++;
	    break;
	  }
       }
     }
     else {
       /***********************************************************************
       * A non-zero source byte - write it to the destination file.
       ***********************************************************************/
       if (!WRITEv64(&byte,1,1,destFp)) return destError;
       (*destSize)++;
     }
  }
#else
  return UNKNOWN_COMPRESSION;
#endif
}

/******************************************************************************
* DecompressRLE0_64.
******************************************************************************/

STATICforIDL CDFstatus DecompressRLE0_64 (srcFp, srcOffset, srcSize, srcError,
				          destFp, destOffset, destError)
vFILE *srcFp;
OFF_T srcOffset;
OFF_T srcSize;
CDFstatus srcError;
vFILE *destFp;
OFF_T destOffset;
CDFstatus destError;
{
#if SUPPORT_RLE64
  CDFstatus pStatus = CDF_OK; Int32 byteN = 0, count, i;
  uByte byte, zero = 0, zCount;
  /****************************************************************************
  * Seek to starting offsets.
  ****************************************************************************/
  if (!SEEKv64(srcFp,srcOffset,vSEEK_SET)) return srcError;
  if (!SEEKv64(destFp,destOffset,vSEEK_SET)) return destError;
  /****************************************************************************
  * Begin...
  ****************************************************************************/
  for (;;) {
     /*************************************************************************
     * Return if past the last source byte
     *************************************************************************/
     if (byteN == srcSize) return pStatus;
     /*************************************************************************
     * Read a source byte.  If it is a zero, read the following count byte
     * and write the specified number of zeroes to the destination file.
     * Otherwise, just write the non-zero byte.  Note that the count byte is
     * one less than the number of zeroes.
     *************************************************************************/
     if (!READv64(&byte,1,1,srcFp)) return srcError;
     byteN++;
     if (byte == 0) {
       if (byteN == srcSize) return DECOMPRESSION_ERROR;
       if (!READv64(&zCount,1,1,srcFp)) return srcError;
       byteN++;
       count = ((Int32) zCount) + 1;
       for (i = 0; i < count; i++) {
	  if (!WRITEv64(&zero,1,1,destFp)) return destError;
       }
     }
     else
       if (!WRITEv64(&byte,1,1,destFp)) return destError;
  }
#else
  return UNKNOWN_COMPRESSION;
#endif
}
