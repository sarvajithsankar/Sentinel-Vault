#ifndef VAULTHEADER_H
#define VAULTHEADER_H

struct VaultHeader {
    char magic[8];
    unsigned int version;
    unsigned int recordCount;
    long long lastSyncTime;
};
#endif 
