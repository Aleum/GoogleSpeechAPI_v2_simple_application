#pragma comment(lib, "winmm.lib")
#include "main.h"

extern "C" {
	#include <curl.h>
}

// input data format
struct WriteThis {
	const char *readptr;
	long sizeleft;
};

// callback function from http://curl.haxx.se/libcurl/c/post-callback.html
static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
	struct WriteThis *pooh = (struct WriteThis *)userp;

	if (size*nmemb < 1)
		return 0;

	if (pooh->sizeleft) {
		*(char *)ptr = pooh->readptr[0]; /* copy one single byte */
		pooh->readptr++;                 /* advance pointer */
		pooh->sizeleft--;                /* less data left */
		return 1;                        /* we return 1 byte at a time! */
	}

	return 0;                          /* no more data left to deliver */
}

// result callback function
size_t writer(char *data, size_t size, size_t nmemb, string *writerData)
{
	if (writerData == NULL)
		return 0;

	writerData->append(data, size*nmemb);
	return size * nmemb;
}

string HOST_ADDR = "https://www.google.com/speech-api/v2/recognize?output=json&lang=ko-kr&key=";
char* ContentType = "Content-Type: audio/l16; rate=16000; charset=utf-8;";
string mykey = "*******";

string HTTPCommModule::startHTTPComm(char* fpath)
{

	CURL *curl;         // curl handle
    CURLcode res;
	string html;

	HOST_ADDR += mykey;

    curl = curl_easy_init();
    if (curl) 
    {
        FILE *file;
        errno_t err = fopen_s(&file, fpath, "r");
        fseek(file, 0, SEEK_END);
        int fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        std::cout << "file open status: " << err << std::endl;
        std::cout << "file " << fileSize << std::endl;

        struct curl_slist *chunk = NULL;

		string Result;          // string which will contain the result
		ostringstream convert;   // stream used for the conversion
		convert << fileSize;      // insert the textual representation of 'Number' in the characters in the stream
		Result = convert.str(); // set 'Result' to the contents of the stream

		string sizeHeader = "Content-Length: ";
		sizeHeader += Result;

		chunk = curl_slist_append(chunk, ContentType);
		chunk = curl_slist_append(chunk, sizeHeader.c_str());

        char *audioData = (char*)malloc(fileSize);
		struct WriteThis pooh;
		fread(audioData, fileSize, 1, file);
		fclose(file);

		pooh.readptr = audioData;
		pooh.sizeleft = fileSize;

        std::cout << curl_easy_setopt(curl, CURLOPT_POST, 1L) << std::endl;
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
		curl_easy_setopt(curl, CURLOPT_READDATA, &pooh);
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0);
		curl_easy_setopt(curl, CURLOPT_PORT, 443);
		curl_easy_setopt(curl, CURLOPT_ENCODING, "UTF-8");
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);

        std::cout << curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk) << std::endl;
        std::cout << curl_easy_setopt(curl, CURLOPT_URL, HOST_ADDR.c_str()) << std::endl;
		
        std::cout << "setup finish..."<<std::endl;
        res = curl_easy_perform(curl);
        std::cout << "perform finish..."<<std::endl;
        curl_easy_cleanup(curl);
		curl_global_cleanup();

    }
    return html;
	
}

int main()
{
	// test
	HTTPCommModule module;
	string result = module.startHTTPComm("voice.wav");

	// save result to file
	ofstream file("result.txt");
	file << result;
	file.close();

	return 0;

}
