# C-Service-Interface(CSIF)
C service interface is C Language based, It is a Service Interface build on top of FastCGI and Nginx which provided function handler support, it also provided useful collection tools to enhance the facilities. 

## Prerequisition Installed
Nginx -- https://www.nginx.com/

cJSON libraries -- https://github.com/DaveGamble/cJSON

fcgi library -- https://github.com/FastCGI-Archives/FastCGI.com

cmake and make

### Supported OS: LINUX

## Step of installation
### 1. Go to root directory
### 2. type 
> mkdir build (if not existed)

> cd build

> cmake ..

> make

> sudo make install


### 3. the result will be
Install the project...

-- Install configuration: ""

-- Installing: /usr/local/lib/libcsif.dylib

-- Installing: /usr/local/lib/libcsif.a

-- Installing: /usr/local/include/csif/csif.h

-- Installing: /usr/local/include/csif/csif_json.h

-- Installing: /usr/local/include/csif/csif_hash.h

-- Installing: /usr/local/include/csif/csif_LFHashTable.h

-- Installing: /usr/local/include/csif/csif_map.h

-- Installing: /usr/local/include/csif/csif_buf.h

-- Installing: /usr/local/include/csif/csif_pool.h


### 4. build a simple program by execute 

> gcc ../services_sample/profile_service.c -lcsif -lcjson -lfcgi -rdynamic -o simple_service


### 5. when you type 

> ./simple_service

it will result:-
Available options:

	-a	the ip address which binding to
	
	-p	port number to specified, not for -s
	
	-s	unix domain socket path to generate, not for -p
	
	-q	number of socket backlog
	
	-w	number of worker process
	
	-l	log file path
	
	-e	signal handling
	
	-f	Fork Daemon process
	
	-d	Run on debug Mode
	
	-o	Dynamic Link shared object file
	
	-h	display usage
	
	-v	display version
	

### 6. simple start a service by execute 

> ./simple_service -p2005 -q200 -w200 -d


### 7. Edit the nginx.conf in your nginx config folder by append in your server block:-

	location /getProfile {
      add_header Allow "GET, POST, HEAD" always;
      if ( $request_method !~ ^(GET|HEAD)$ ) {
        return 405;
      }
      include /etc/nginx/fastcgi_params;
      fastcgi_param FN_HANDLER getProfile;
      fastcgi_pass 127.0.0.1:2005;
    }

    location /postProfile {
      add_header Allow "GET, POST, HEAD" always;
      if ( $request_method !~ ^(POST)$ ) {
        return 405;
      }
       include /etc/nginx/fastcgi_params;
      fastcgi_param FN_HANDLER postProfile;
      fastcgi_pass 127.0.0.1:2005;
    }

***You will see the FN_HANDLER is function name mapping with the function inside simple_service code, the fastcgi port 2005 is the service you start with(please look at step 10 for more details.***


### 8. start the nginx server

### 9.  Using apache benchmark for get request load test

> ab -c 100 -n 10000 http://127.0.0.1:80/getProfile


#### For post request load test

> ab -p "payload.txt" -T application/json -c 100 -n 10000 http://127.0.0.1:80/postProfile

*the payload.txt is inside the root directory*


## To uninstall.
### 1. Go to root_directory/build folder -- make sure build content is still existed.
### 2. type "sudo make uninstall" 
Then result

-- Uninstalling /usr/local/lib/libcsif.dylib

-- Uninstalling /usr/local/lib/libcsif.a

-- Uninstalling /usr/local/include/csif/csif.h

-- Uninstalling /usr/local/include/csif/csif_json.h

-- Uninstalling /usr/local/include/csif/csif_hash.h

-- Uninstalling /usr/local/include/csif/csif_LFHashTable.h

-- Uninstalling /usr/local/include/csif/csif_map.h

-- Uninstalling /usr/local/include/csif/csif_buf.h

-- Uninstalling /usr/local/include/csif/csif_pool.h

