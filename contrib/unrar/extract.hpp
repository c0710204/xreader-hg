#ifndef _RAR_EXTRACT_
#define _RAR_EXTRACT_

#include "boost/scoped_ptr.hpp"
using boost::scoped_ptr;

enum EXTRACT_ARC_CODE {EXTRACT_ARC_NEXT,EXTRACT_ARC_REPEAT};

class CmdExtract
{
  private:
    EXTRACT_ARC_CODE ExtractArchive(CommandData *Cmd);
    RarTime StartTime; // time when extraction started

    ComprDataIO DataIO;
    scoped_ptr<Unpack> Unp;
    unsigned long TotalFileCount;

    unsigned long FileCount;
    unsigned long MatchedArgs;
    bool FirstFile;
    bool AllMatchesExact;
    bool ReconstructDone;

    // If any non-zero solid file was successfully unpacked before current.
    // If true and if current encrypted file is broken, obviously
    // the password is correct and we can report broken CRC without
    // any wrong password hints.
    bool AnySolidDataUnpackedWell;

    char ArcName[NM];
    wchar ArcNameW[NM];

    wchar Password[MAXPASSWORD];
    bool PasswordAll;
    bool PrevExtracted;
    char DestFileName[NM];
    wchar DestFileNameW[NM];
    bool PasswordCancelled;
  public:
    CmdExtract();
    ~CmdExtract();
    void DoExtract(CommandData *Cmd);
    void ExtractArchiveInit(CommandData *Cmd,Archive &Arc);
    bool ExtractCurrentFile(CommandData *Cmd,Archive &Arc,size_t HeaderSize,
                            bool &Repeat);
    static void UnstoreFile(ComprDataIO &DataIO,int64 DestUnpSize);

    bool SignatureFound;
};

#endif
