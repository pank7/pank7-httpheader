/*
 * File:                httpheader.h
 * Description:         HTTP header parse and process.
 * Author:              Yi Li <liyi84@gmail.com>
 * Date & Time          Wed Aug 18 18:09:28 2010
 */

#ifndef _HTTPHEADER_H_
#define _HTTPHEADER_H_

#include        <sys/types.h>
#include        "list.h"

#ifdef  __cplusplus
extern "C" {
#endif  /* __cplusplus */

#define MAXREQLEN       4096
#define MAXRESPLEN      4096
#define MAXURILEN       2048
#define MAXEPATHLEN     4096
#define MAXFILELEN      (1024 * 1024 - 1)

  /*
   * OCTET   = <any 8-bit sequence of data>
   * CHAR    = <any US-ASCII character (octets 0 - 127)>
   * UPALPHA = <any US-ASCII uppercase letter "A".."Z">
   * LOALPHA = <any US-ASCII lowercase letter "a".."z">
   * ALPHA   = UPALPHA | LOALPHA
   * DIGIT   = <any US-ASCII digit "0".."9">
   * CTL     = <any US-ASCII control character (octets 0 - 31) and DEL (127)>
   * CR      = <US-ASCII CR, carriage return (13)>
   * LF      = <US-ASCII LF, linefeed (10)>
   * SP      = <US-ASCII SP, space (32)>
   * HT      = <US-ASCII HT, horizontal-tab (9)>
   * <">     = <US-ASCII double-quote mark (34)>
   *
   * CRLF = CR LF
   * LWS = [CRLF] 1*( SP | HT )
   * TEXT = <any OCTET except CTLs, but including LWS>
   * HEX = "A" | "B" | "C" | "D" | "E" | "F" | "a"
   *     | "b" | "c" | "d" | "e" | "f"
   *     | DIGIT
   * token      = 1*<any CHAR except CTLs or separators>
   * separators = "(" | ")" | "<" | ">" | "@" | "," | ";" | ":" | "\" | <">
   *            | "/" | "[" | "]" | "?" | "=" | "{" | "}" | SP | HT
   * comment = "(" *( ctext | quoted-pair | comment ) ")"
   * ctext   = <any TEXT excluding "(" and ")">
   * quoted-string  = ( <"> *(qdtext | quoted-pair ) <"> )
   * qdtext         = <any TEXT except <">>
   * quoted-pair = "\" CHAR
   */
  
  /*
   * HTTP request format:
   * 
   * Request = Request-Line
   *           *(( general-header
   *           | request-header
   *           | entity-header ) CRLF)
   *           CRLF
   *           [ message-body ]
   */

  /*
   * Status-Code = "100"  ; Continue
   *             | "101"  ; Switching Protocols
   *             | "200"  ; OK
   *             | "202"  ; Accepted
   *             | "203"  ; Non-Authoritative Information
   *             | "204"  ; No Content
   *             | "205"  ; Reset Content
   *             | "206"  ; Partial Content
   *             | "300"  ; Multiple Choices
   *             | "301"  ; Moved Permanently
   *             | "302"  ; Found
   *             | "303"  ; See Other
   *             | "304"  ; Not Modified
   *             | "305"  ; Use Proxy
   *             | "307"  ; Temporary Redirect
   *             | "400"  ; Bad Request
   *             | "401"  ; Unauthorized
   *             | "402"  ; Payment Required
   *             | "403"  ; Forbidden
   *             | "404"  ; Not Found
   *             | "405"  ; Method Not Allowed
   *             | "406"  ; Not Acceptable
   *             | "407"  ; Proxy Authentication Required
   *             | "408"  ; Request Time-out
   *             | "409"  ; Conflict
   *             | "410"  ; Gone
   *             | "411"  ; Length Required
   *             | "412"  ; Precondition Failed
   *             | "413"  ; Request Entity Too Large
   *             | "414"  ; Request-URI Too Large
   *             | "415"  ; Unsupported Media Type
   *             | "416"  ; Requested range not satisfiable
   *             | "417"  ; Expectation Failed
   *             | "500"  ; Internal Server Error
   *             | "501"  ; Not Implemented
   *             | "502"  ; Bad Gateway
   *             | "503"  ; Service Unavailable
   *             | "504"  ; Gateway Time-out
   *             | "505"  ; HTTP Version not supported
   *             | extension-code
   *
   * extension-code = 3DIGIT
   * Reason-Phrase  = *<TEXT, excluding CR, LF>
   */
  enum _http_status_t
    {
      HTTP_STATUS_100,          /* Continue */
      HTTP_STATUS_101,          /* Switching Protocols */
      HTTP_STATUS_200,          /* OK */
      HTTP_STATUS_202,          /* Accepted */
      HTTP_STATUS_203,          /* Non-Authoritative Information */
      HTTP_STATUS_204,          /* No Content */
      HTTP_STATUS_205,          /* Reset Content */
      HTTP_STATUS_206,          /* Partial Content */
      HTTP_STATUS_300,          /* Multiple Choices */
      HTTP_STATUS_301,          /* Moved Permanently */
      HTTP_STATUS_302,          /* Found */
      HTTP_STATUS_303,          /* See Other */
      HTTP_STATUS_304,          /* Not Modified */
      HTTP_STATUS_305,          /* Use Proxy */
      HTTP_STATUS_307,          /* Temporary Redirect */
      HTTP_STATUS_400,          /* Bad Request */
      HTTP_STATUS_401,          /* Unauthorized */
      HTTP_STATUS_402,          /* Payment Required */
      HTTP_STATUS_403,          /* Forbidden */
      HTTP_STATUS_404,          /* Not Found */
      HTTP_STATUS_405,          /* Method Not Allowed */
      HTTP_STATUS_406,          /* Not Acceptable */
      HTTP_STATUS_407,          /* Proxy Authentication Required */
      HTTP_STATUS_408,          /* Request Time-out */
      HTTP_STATUS_409,          /* Conflict */
      HTTP_STATUS_410,          /* Gone */
      HTTP_STATUS_411,          /* Length Required */
      HTTP_STATUS_412,          /* Precondition Failed */
      HTTP_STATUS_413,          /* Request Entity Too Large */
      HTTP_STATUS_414,          /* Request-URI Too Large */
      HTTP_STATUS_415,          /* Unsupported Media Type */
      HTTP_STATUS_416,          /* Requested range not satisfiable */
      HTTP_STATUS_417,          /* Expectation Failed */
      HTTP_STATUS_500,          /* Internal Server Error */
      HTTP_STATUS_501,          /* Not Implemented */
      HTTP_STATUS_502,          /* Bad Gateway */
      HTTP_STATUS_503,          /* Service Unavailable */
      HTTP_STATUS_504,          /* Gateway Time-out */
      HTTP_STATUS_505,          /* HTTP version not supported */
      HTTP_STATUS_UNSET         /* Unset */
    };
  typedef enum _http_status_t http_status_t;
  struct _status_explain_t 
  {
    http_status_t       status;
    char                *code;
    char                *explain;
  };
  typedef struct _status_explain_t status_explain_t;
  extern status_explain_t       se[];
  extern size_t                 senr;

  /*
   * Method = "OPTIONS"
   *        | "GET"
   *        | "HEAD"
   *        | "POST"
   *        | "PUT"
   *        | "DELETE"
   *        | "TRACE"
   *        | "CONNECT"
   *        | extension-method
   * extension-method = token
   */
  enum _http_method_t
    {
      HTTP_METHOD_OPTION,
      HTTP_METHOD_GET,
      HTTP_METHOD_HEAD,
      HTTP_METHOD_POST,
      HTTP_METHOD_PUT,
      HTTP_METHOD_DELETE,
      HTTP_METHOD_TRACE,
      HTTP_METHOD_CONNECT,
      HTTP_METHOD_UNSET
    };
  typedef enum _http_method_t http_method_t;
  struct _method_name_t
  {
    http_method_t       method;
    char                *name;
  };
  typedef struct _method_name_t method_name_t;
  extern method_name_t          mn[];
  extern size_t                 mnnr;

  enum _http_version_t
    {
      HTTP_VERSION_0_9,
      HTTP_VERSION_1_0,
      HTTP_VERSION_1_1,
      HTTP_VERSION_UNSET
    };
  typedef enum _http_version_t http_version_t;
  struct _version_name_t
  {
    http_version_t      version;
    char                *name;
  };
  typedef struct _version_name_t version_name_t;
  extern version_name_t         vn[];
  extern size_t                 vnnr;

  /* Keep alive? */
  enum _http_connection_t
    {
      HTTP_CONNECTION_KEEPALIVE,
      HTTP_CONNECTION_CLOSE,
      HTTP_CONNECTION_UNSET
    };
  typedef enum _http_connection_t http_connection_t;
  struct _connection_name_t
  {
    http_connection_t   connection;
    char                *name;
  };
  typedef struct _connection_name_t connection_name_t;
  extern connection_name_t      cn[];
  extern size_t                 cnnr;

  /*
   * general-header = Cache-Control
   *                | Connection
   *                | Date
   *                | Pragma
   *                | Trailer
   *                | Transfer-Encoding
   *                | Upgrade
   *                | Via
   *                | Warning
   */
  enum _general_header_t
  {
    GENERAL_HEADER_CACHECONTROL,
    GENERAL_HEADER_CONNECTION,
    GENERAL_HEADER_DATE,
    GENERAL_HEADER_PRAGMA,
    GENERAL_HEADER_TRAILER,
    GENERAL_HEADER_TRANSFERENC,
    GENERAL_HEADER_UPGRADE,
    GENERAL_HEADER_VIA,
    GENERAL_HEADER_WARNING
  };
  typedef enum _general_header_t general_header_t;
  struct _gh_name_t
  {
    general_header_t    header;
    char                *name;
  };
  typedef struct _gh_name_t gh_name_t;
  extern gh_name_t      ghn[];
  extern size_t         ghnr;

  /* request-header = Accept
   *                | Accept-Charset
   *                | Accept-Encoding
   *                | Accept-Language
   *                | Authorization
   *                | Expect
   *                | From
   *                | Host
   *                | If-Match
   *                | If-Modified-Since
   *                | If-None-Match
   *                | If-Range
   *                | If-Unmodified-Since
   *                | Max-Forwards
   *                | Proxy-Authorization
   *                | Range
   *                | Referer
   *                | TE
   *                | User-Agent
   */
  enum _request_header_t
  {
    REQUEST_HEADER_ACCEPT,
    REQUEST_HEADER_ACCEPTCHAR,
    REQUEST_HEADER_ACCEPTENC,
    REQUEST_HEADER_ACCEPTLANG,
    REQUEST_HEADER_AUTHORIZATION,
    REQUEST_HEADER_EXPECT,
    REQUEST_HEADER_FROM,
    REQUEST_HEADER_HOST,
    REQUEST_HEADER_IFMATCH,
    REQUEST_HEADER_IFMODSIN,
    REQUEST_HEADER_IFNONMAT,
    REQUEST_HEADER_IFRANGE,
    REQUEST_HEADER_IFUNMODSIN,
    REQUEST_HEADER_MAXFORWARDS,
    REQUEST_HEADER_PROXYAUTH,
    REQUEST_HEADER_RANGE,
    REQUEST_HEADER_REFERER,
    REQUEST_HEADER_TE,
    REQUEST_HEADER_USERAGENT
  };
  typedef enum _request_header_t request_header_t;
  struct _reqh_name_t
  {
    request_header_t    header;
    char                *name;
  };
  typedef struct _reqh_name_t reqh_name_t;
  extern reqh_name_t    reqhn[];
  extern size_t         reqhnr;

  /* entity-header = Allow
   *               | Content-Encoding
   *               | Content-Language
   *               | Content-Length
   *               | Content-Location
   *               | Content-MD5
   *               | Content-Range
   *               | Content-Type
   *               | Expires
   *               | Last-Modified
   *               | extension-header
   * extension-header = message-header
   */
  enum _entity_header_t
    {
      ENTITY_HEADER_ALLOW,
      ENTITY_HEADER_CONENC,
      ENTITY_HEADER_CONLANG,
      ENTITY_HEADER_CONLENGTH,
      ENTITY_HEADER_CONLOC,
      ENTITY_HEADER_CONMD5,
      ENTITY_HEADER_CONRANGE,
      ENTITY_HEADER_CONTYPE,
      ENTITY_HEADER_EXPIRES,
      ENTITY_HEADER_LASTMOD
    };
  typedef enum _entity_header_t entity_header_t;
  struct _eh_name_t
  {
    entity_header_t     header;
    char                *name;
  };
  typedef struct _eh_name_t eh_name_t;
  extern eh_name_t      ehn[];
  extern size_t         ehnr;

  /* response-header = Accept-Ranges
   *                 | Age
   *                 | ETag
   *                 | Location
   *                 | Proxy-Authenticate
   *                 | Retry-After
   *                 | Server
   *                 | Vary
   *                 | WWW-Authenticate
   */
  enum _response_header_t
    {
      RESPONSE_HEADER_ACCRANGE,
      RESPONSE_HEADER_AGE,
      RESPONSE_HEADER_ETAG,
      RESPONSE_HEADER_LOCATION,
      RESPONSE_HEADER_PROXYAUTHEN,
      RESPONSE_HEADER_RETRYAFTER,
      RESPONSE_HEADER_SERVER,
      RESPONSE_HEADER_VARY,
      RESPONSE_HEADER_WWWAUTHEN
    };
  typedef enum _response_header_t response_header_t;
  struct _resph_name_t
  {
    response_header_t   header;
    char                *name;
  };
  typedef struct _resph_name_t resph_name_t;
  extern resph_name_t   resphn[];
  extern size_t         resphnr;

  /* HTTP header */
  struct _header_t
  {
    char        *key;
    char        *val;
    list_head   hook;
  };
  typedef struct _header_t header_t;

  void
  header_init (header_t *header);

  void
  header_set (header_t *header, char *key, char *val);

  void
  header_delete (header_t *header);

  /*
   * Response = Status-Line
   *          *(( general-header
   *          | response-header
   *          | entity-header ) CRLF)
   *          CRLF
   *          [ message-body ]
   *
   * Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
   */
  struct _response_t 
  {
    http_version_t      version;
    http_status_t       status;
    char                *reason;
    char                *server;
    char                *location;
    http_connection_t   keep_alive;
    int                 chunked;
    time_t              date;
    time_t              lmt;
    off_t               contentlength;
    list_head           headers;
  };
  typedef struct _response_t response_t;

  void
  response_init (response_t *response);

  void
  response_delete (response_t *response);

  void
  response_set_reason (response_t *response, char *reason, size_t rlen);

  void
  response_set_server (response_t *response, char *server, size_t slen);

  void
  response_set_location (response_t *response, char *location, size_t llen);

  void
  response_show (response_t *response);

  void
  response_add_headerkv (response_t *response, char *key, char *val);

  /* Parse HTTP response message. */
  int
  http_response_parse (const char *respstr, size_t resplen,
                       response_t *response);

  /* Check validity of HTTP response. */
  int
  http_response_check (response_t *response);

#ifdef  __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _HTTPHEADER_H_ */
