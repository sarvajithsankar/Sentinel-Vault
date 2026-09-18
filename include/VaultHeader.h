#ifndef VAULTHEADER_H
#define VAULTHEADER_H

struct VaultHeader{
    unsigned int isencrypted : 1;
    unsigned int iscompressed : 1;
    unsigned int integritypassed : 1;
    
    unsigned int version : 5;

};
#endif 
