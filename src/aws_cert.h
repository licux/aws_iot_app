#ifndef __AWS_CERT_H__
#define __AWS_CERT_H__

#define ADDRESS "ssl://xxx.amazonaws.com:8883"


#define CERFILE_PATH "../AWSIoT_myDevice/"

#define root_ca CERFILE_PATH "AmazonRootCA1.pem"
#define certificate  CERFILE_PATH "xxx-certificate.pem.crt"
#define private_key  CERFILE_PATH "xxx-private.pem.key"
#define ca_path CERFILE_PATH "ca/"

#endif 
