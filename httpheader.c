#include        <stdio.h>
#include        <sys/types.h>
#include        <sys/stat.h>
#include        <unistd.h>
#include        <time.h>
#include        <limits.h>
#include        <stdlib.h>
#include        <string.h>
#include        <strings.h>
#include        <ctype.h>
#include        <assert.h>
#include        <error.h>
#include        <errno.h>

#ifdef  HAVE_CONFIG_H
#include        "config.h"
#endif  /* HAVE_CONFIG_H */

#include        "httpheader.h"

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
status_explain_t        se[] =
  {
    {HTTP_STATUS_100, "100", "Continue"},
    {HTTP_STATUS_101, "101", "Switching Protocols"},
    {HTTP_STATUS_200, "200", "OK"},
    {HTTP_STATUS_202, "202", "Accepted"},
    {HTTP_STATUS_203, "203", "Non-Authoritative Information"},
    {HTTP_STATUS_204, "204", "No Content"},
    {HTTP_STATUS_205, "205", "Reset Content"},
    {HTTP_STATUS_206, "206", "Partial Content"},
    {HTTP_STATUS_300, "300", "Multiple Choices"},
    {HTTP_STATUS_301, "301", "Moved Permanently"},
    {HTTP_STATUS_302, "302", "Found"},
    {HTTP_STATUS_303, "303", "See Other"},
    {HTTP_STATUS_304, "304", "Not Modified"},
    {HTTP_STATUS_305, "305", "Use Proxy"},
    {HTTP_STATUS_307, "307", "Temporary Redirect"},
    {HTTP_STATUS_400, "400", "Bad Request"},
    {HTTP_STATUS_401, "401", "Unauthorized"},
    {HTTP_STATUS_402, "402", "Payment Required"},
    {HTTP_STATUS_403, "403", "Forbidden"},
    {HTTP_STATUS_404, "404", "Not Found"},
    {HTTP_STATUS_405, "405", "Method Not Allowed"},
    {HTTP_STATUS_406, "406", "Not Acceptable"},
    {HTTP_STATUS_407, "407", "Proxy Authentication Required"},
    {HTTP_STATUS_408, "408", "Request Time-out"},
    {HTTP_STATUS_409, "409", "Conflict"},
    {HTTP_STATUS_410, "410", "Gone"},
    {HTTP_STATUS_411, "411", "Length Required"},
    {HTTP_STATUS_412, "412", "Precondition Failed"},
    {HTTP_STATUS_413, "413", "Request Entity Too Large"},
    {HTTP_STATUS_414, "414", "Request-URI Too Large"},
    {HTTP_STATUS_415, "415", "Unsupported Media Type"},
    {HTTP_STATUS_416, "416", "Requested range not satisfiable"},
    {HTTP_STATUS_417, "417", "Expectation Failed"},
    {HTTP_STATUS_500, "500", "Internal Server Error"},
    {HTTP_STATUS_501, "501", "Not Implemented"},
    {HTTP_STATUS_502, "502", "Bad Gateway"},
    {HTTP_STATUS_503, "503", "Service Unavailable"},
    {HTTP_STATUS_504, "504", "Gateway Time-out"},
    {HTTP_STATUS_505, "505", "HTTP version not supported"},
    {HTTP_STATUS_UNSET, "600", "Don't fuck me..."}
  };
size_t                  senr = 39;

method_name_t           mn[] =
  {
    {HTTP_METHOD_OPTION, "OPTIONS"},
    {HTTP_METHOD_GET, "GET"},
    {HTTP_METHOD_HEAD, "HEAD"},
    {HTTP_METHOD_POST, "POST"},
    {HTTP_METHOD_PUT, "PUT"},
    {HTTP_METHOD_DELETE, "DELETE"},
    {HTTP_METHOD_TRACE, "TRACE"},
    {HTTP_METHOD_CONNECT, "CONNECT"},
    {HTTP_METHOD_UNSET, "FUCK"}
  };
size_t                  mnnr = 8;
    
version_name_t          vn[] =
  {
    {HTTP_VERSION_0_9, "HTTP/0.9"},
    {HTTP_VERSION_1_0, "HTTP/1.0"},
    {HTTP_VERSION_1_1, "HTTP/1.1"},
    {HTTP_VERSION_UNSET, "HTTP/FUCK"}
  };
size_t                  vnnr = 3;

connection_name_t       cn[] =
  {
    {HTTP_CONNECTION_KEEPALIVE, "Keep-Alive"},
    {HTTP_CONNECTION_CLOSE, "Close"},
    {HTTP_CONNECTION_UNSET, "Fuck"}
  };
size_t                  cnnr = 2;

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
gh_name_t               ghn[] =
  {
    {GENERAL_HEADER_CACHECONTROL, "Cache-Control"},
    {GENERAL_HEADER_CONNECTION, "Connection"},
    {GENERAL_HEADER_DATE, "Date"},
    {GENERAL_HEADER_PRAGMA, "Pragma"},
    {GENERAL_HEADER_TRAILER, "Trailer"},
    {GENERAL_HEADER_TRANSFERENC, "Transfer-Encoding"},
    {GENERAL_HEADER_UPGRADE, "Upgrade"},
    {GENERAL_HEADER_VIA, "Via"},
    {GENERAL_HEADER_WARNING, "Warning"}
  };
size_t          ghnr = 9;

/*
 * request-header = Accept
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
reqh_name_t             reqhn[] =
  {
    {REQUEST_HEADER_ACCEPT, "Accept"},
    {REQUEST_HEADER_ACCEPTCHAR, "Accept-Charset"},
    {REQUEST_HEADER_ACCEPTENC, "Accept-Encoding"},
    {REQUEST_HEADER_ACCEPTLANG, "Accept-Language"},
    {REQUEST_HEADER_AUTHORIZATION, "Authorization"},
    {REQUEST_HEADER_EXPECT, "Expect"},
    {REQUEST_HEADER_FROM, "From"},
    {REQUEST_HEADER_HOST, "Host"},
    {REQUEST_HEADER_IFMATCH, "If-Match"},
    {REQUEST_HEADER_IFMODSIN, "If-Modified-Since"},
    {REQUEST_HEADER_IFNONMAT, "If-None-Match"},
    {REQUEST_HEADER_IFRANGE, "If-Range"},
    {REQUEST_HEADER_IFUNMODSIN, "If-Unmodified-Since"},
    {REQUEST_HEADER_MAXFORWARDS, "Max-Forwards"},
    {REQUEST_HEADER_PROXYAUTH, "Proxy-Authorization"},
    {REQUEST_HEADER_RANGE, "Range"},
    {REQUEST_HEADER_REFERER, "Referer"},
    {REQUEST_HEADER_TE, "TE"},
    {REQUEST_HEADER_USERAGENT, "User-Agent"}
  };
size_t                  reqhnr = 19;

/*
 * entity-header = Allow
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
eh_name_t       ehn[] =
  {
    {ENTITY_HEADER_ALLOW, "Allow"},
    {ENTITY_HEADER_CONENC, "Content-Encoding"},
    {ENTITY_HEADER_CONLANG, "Content-Language"},
    {ENTITY_HEADER_CONLENGTH, "Content-Length"},
    {ENTITY_HEADER_CONLOC, "Content-Location"},
    {ENTITY_HEADER_CONMD5, "Content-MD5"},
    {ENTITY_HEADER_CONRANGE, "Content-Range"},
    {ENTITY_HEADER_CONTYPE, "Content-Type"},
    {ENTITY_HEADER_EXPIRES, "Expires"},
    {ENTITY_HEADER_LASTMOD, "Last-Modified"}
  };
size_t                  ehnr = 10;

/*
 * response-header = Accept-Ranges
 *                 | Age
 *                 | ETag
 *                 | Location
 *                 | Proxy-Authenticate
 *                 | Retry-After
 *                 | Server
 *                 | Vary
 *                 | WWW-Authenticate
 */
resph_name_t            resphn[] =
  {
    {RESPONSE_HEADER_ACCRANGE, "Accept-Ranges"},
    {RESPONSE_HEADER_AGE, "Age"},
    {RESPONSE_HEADER_ETAG, "ETag"},
    {RESPONSE_HEADER_LOCATION, "Location"},
    {RESPONSE_HEADER_PROXYAUTHEN, "Proxy-Authenticate"},
    {RESPONSE_HEADER_RETRYAFTER, "Retry-After"},
    {RESPONSE_HEADER_SERVER, "Server"},
    {RESPONSE_HEADER_VARY, "Vary"},
    {RESPONSE_HEADER_WWWAUTHEN, "WWW-Authenticate"}
  };
size_t                  resphnr = 9;

void
header_init (header_t *header)
{
  if (!header) return;

  header->key = NULL;
  header->val = NULL;
  INIT_LIST_HEAD (&header->hook);

  return;
}

void
header_set (header_t *header, char *key, char *val)
{
  if (!header || !key || !val) return;

  size_t        keylen = strlen (key);
  size_t        vallen = strlen (val);

  if (header->key) free (key);
  if (header->val) free (val);

  header->key = (char *)malloc (sizeof (char) * (keylen + 1));
  assert (header->key);
  header->val = (char *)malloc (sizeof (char) * (vallen + 1));
  assert (header->val);

  strncpy (header->key, key, keylen);
  header->key[keylen] = '\0';

  strncpy (header->val, val, vallen);
  header->val[vallen] = '\0';

  return;
}

void
header_delete (header_t *header)
{
  if (!header) return;

  if (header->key) free (header->key);
  if (header->val) free (header->val);
  header->key = NULL;
  header->val = NULL;

  return;
}

void
response_show (response_t *response)
{
  if (!response) return;

  fprintf (stderr, "Status: %s %s\n",
           se[response->status].code,
           se[response->status].explain);
  fprintf (stderr, "HTTP-Version: %s\n", vn[response->version].name);
  if (response->server)
    fprintf (stderr, "Server: %s\n", response->server);
  fprintf (stderr, "Connection: %s\n", cn[response->keep_alive].name);
  fprintf (stderr, "Content-Length: %lu\n", response->contentlength);
  if (response->location)
    fprintf (stderr, "Location: %s\n", response->location);

  return;
}

void
response_init (response_t *response)
{
  if (!response) return;

  response->version = HTTP_VERSION_1_1;
  response->status = HTTP_STATUS_UNSET;
  response->reason = NULL;
  response->server = NULL;
  response->location = NULL;
  response->keep_alive = HTTP_CONNECTION_UNSET;
  response->chunked = 0;
  response->contentlength = 0;
  INIT_LIST_HEAD (&response->headers);

  return;
}

void
response_delete (response_t *response)
{
  list_head     *node = NULL;
  header_t      *h = NULL;

  if (!response) return;
  if (response->reason) free (response->reason);
  if (response->server) free (response->server);
  if (response->location) free (response->location);
  response->reason = NULL;
  response->server = NULL;
  response->location = NULL;

  while (!list_empty (&response->headers)) {
    node = response->headers.next;
    list_del (response->headers.next);
    h = list_entry (node, header_t, hook);
    header_delete (h);
    free (h);
  } /* while */

  return;
}

void
response_set_location (response_t *response, char *location, size_t llen)
{

  if (!response || !location) return;

  if (response->location) free (response->location);

  response->location = (char *)malloc (sizeof (char) * (llen + 1));
  assert (response->location);
  strncpy (response->location, location, llen);
  response->location[llen] = '\0';

  return;
}

void
response_set_server (response_t *response, char *server, size_t slen)
{

  if (!response || !server) return;

  if (response->server) free (response->server);

  response->server = (char *)malloc (sizeof (char) * (slen + 1));
  assert (response->server);
  strncpy (response->server, server, slen);
  response->server[slen] = '\0';

  return;
}

void
response_set_reason (response_t *response, char *reason, size_t rlen)
{

  if (!response || !reason) return;

  if (response->reason) free (response->reason);

  response->reason = (char *)malloc (sizeof (char) * (rlen + 1));
  assert (response->reason);
  strncpy (response->reason, reason, rlen);
  response->reason[rlen] = '\0';

  return;
}

void
response_add_headerkv (response_t *response, char *key, char *val)
{
  if (!response || !key || !val) return;

  header_t      *header = NULL;

  header = (header_t *)malloc (sizeof (header_t));

  header_init (header);

  header_set (header, key, val);

  list_add_tail (&header->hook, &response->headers);

  return;
}

/*
 * RFC2616: HTTP response format:
 * 
 * Response = Status-Line
 *          *(( general-header
 *          | response-header
 *          | entity-header ) CRLF)
 *          CRLF
 *          [ message-body ]
 */
int
http_response_parse (const char *respstr, size_t resplen,
                     response_t *response)
{
  char          *ptr = (char *)respstr;
  char          *buffer = NULL;
  char          *v = NULL, *s = NULL, *r = NULL;
  size_t        rlen = -1;
  char          c, *headers = NULL;
  int           i = -1, retval = 0;

#ifdef _DEBUG
  fprintf (stderr, "http_response_parse start!\n");
#if     0
  fprintf (stderr, "Status Codes:\n");
  for (i = 0; i < senr; ++i) {
    fprintf (stderr, "%s: %s\n", se[i].code, se[i].explain);
  }
  fprintf (stderr, "Method Names:\n");
  for (i = 0; i < mnnr; ++i) {
    fprintf (stderr, "%d: %s\n", mn[i].method, mn[i].name);
  }
  fprintf (stderr, "Version Names:\n");
  for (i = 0; i < vnnr; ++i) {
    fprintf (stderr, "%d: %s\n", vn[i].version, vn[i].name);
  }
  fprintf (stderr, "Connection Names:\n");
  for (i = 0; i < cnnr; ++i) {
    fprintf (stderr, "%d: %s\n", cn[i].connection, cn[i].name);
  }
  fprintf (stderr, "General Headers:\n");
  for (i = 0; i < ghnr; ++i) {
    fprintf (stderr, "%d: %s\n", ghn[i].header, ghn[i].name);
  }
  fprintf (stderr, "Request Headers:\n");
  for (i = 0; i < reqhnr; ++i) {
    fprintf (stderr, "%d: %s\n", reqhn[i].header, reqhn[i].name);
  }
  fprintf (stderr, "Response Headers:\n");
  for (i = 0; i < resphnr; ++i) {
    fprintf (stderr, "%d: %s\n", resphn[i].header, resphn[i].name);
  }
  fprintf (stderr, "Entity Headers:\n");
  for (i = 0; i < ehnr; ++i) {
    fprintf (stderr, "%d: %s\n", ehn[i].header, ehn[i].name);
  }
#endif  /* 0 */
#endif

  buffer = (char *)malloc (sizeof (char) * (resplen + 1));
  assert (buffer);
  strncpy (buffer, respstr, resplen);
  buffer[resplen] = '\0';
  /*
   * Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
   */
  ptr = strstr (buffer, "\r\n");
  if (!ptr) {
#ifdef  _DEBUG
    fprintf (stderr, "Wrong Status-Line!\n");
#endif  /* _DEBUG */
    retval = -1;
    goto http_response_parse_end;
  }
  *ptr = '\0';
#ifdef  _DEBUG
  fprintf (stderr, "Status-Line: \"%s\"\n", buffer);
#endif  /* _DEBUG */
  headers = ptr + 2;
  while ((*headers == ' ') || (*headers == '\t')) ++headers;
  /* HTTP-Version */
  v = buffer;
  ptr = strpbrk (v, " \t");
  if (!ptr) {
#ifdef  _DEBUG
    fprintf (stderr, "Wrong HTTP-Version!\n");
#endif  /* _DEBUG */
    retval = -1;
    goto http_response_parse_end;
  }
  *ptr = '\0';
  for (i = 0; i < vnnr; ++i) {
    if (strcasecmp (v, vn[i].name) == 0) {
#ifdef _DEBUG
      fprintf (stderr, "HTTP-Version: \"%s\"\n", vn[i].name);
#endif
      response->version = vn[i].version;
      break;
    }
  }
  if (response->version == HTTP_VERSION_UNSET) {
#ifdef _DEBUG
    fprintf (stderr, "HTTP-Version unsupported.\n");
#endif
    retval = -1;
    goto http_response_parse_end;
  }
  /* Status-Code */
  s = ptr + 1;
  while ((*s == ' ') || (*s == '\t')) ++s;
  ptr = strpbrk (s, " \t");
  if (!ptr) {
#ifdef _DEBUG
    fprintf (stderr, "Wrong Status-Code!\n");
#endif
    retval = -1;
    goto http_response_parse_end;
  }
  *ptr = '\0';
  for (i = 0; i < senr; ++i) {
    if (strcasecmp (s, se[i].code) == 0) {
#ifdef _DEBUG
      fprintf (stderr, "Status-Code: \"%s\"\n", se[i].code);
#endif
      response->status = se[i].status;
      break;
    }
  }
  /* Reason-Phrase */
  r = ptr + 1;
  while ((*r == ' ') || (*r == '\t')) ++r;
  rlen = strlen (r);
  response_set_reason (response, r, rlen);
#ifdef _DEBUG
  fprintf (stderr, "Reason: \"%s\"\n", response->reason);
#endif
  
  /*
   * general-header & request-header & entity-header
   *
   * RFC2616:
   *
   * general-header = Cache-Control
   *                | Connection
   *                | Date
   *                | Pragma
   *                | Trailer
   *                | Transfer-Encoding
   *                | Upgrade
   *                | Via
   *                | Warning
   *
   * response-header = Accept-Ranges
   *                 | Age
   *                 | ETag
   *                 | Location
   *                 | Proxy-Authenticate
   *                 | Retry-After
   *                 | Server
   *                 | Vary
   *                 | WWW-Authenticate
   *
   * entity-header = Allow
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
   *
   * extension-header = message-header
   */
  
#define MAXKEYLEN       64
#define MAXVALLEN       4096

  enum header_state
    {
      NEWKEY,
      NEWVAL
    } headerstate;

  char          key[MAXKEYLEN], val[MAXVALLEN];
  size_t        keylen = 0, vallen = 0;

  ptr = headers;
  headerstate = NEWKEY;
  c = *ptr;
  while (c) {
    switch (c) {
    case '\r':
      if (*(ptr + 1) == '\n') {
        ++ptr;
        switch (headerstate) {
        case NEWKEY:
          while ((keylen > 0) &&
                 ((key[keylen - 1] == ' ') ||
                  (key[keylen - 1] == '\t'))) {
            --keylen;
          }
          if (keylen == 0) {
#ifdef _DEBUG
            /* fprintf (stderr, "End of headers!\n"); */
#endif
            retval = 0;
            goto http_response_parse_end;
          } else {
#ifdef _DEBUG
            fprintf (stderr, "Invalid header: DO NOT FUCK WITH ME!\n");
#endif
            retval = -1;
            goto http_response_parse_end;
          }
          break;
        case NEWVAL:
          while ((keylen > 0) &&
                 ((key[keylen - 1] == ' ') ||
                  (key[keylen - 1] == '\t')))
            --keylen;
          if (keylen <= 0) {
#ifdef _DEBUG
            fprintf (stderr, "Invalid header: empty key!\n");
#endif
            retval = -1;
            goto http_response_parse_end;
          }
          while ((vallen > 0) &&
                 ((val[vallen - 1] == ' ') ||
                  (val[vallen - 1] == '\t')))
            --vallen;
          key[keylen] = '\0';
          val[vallen] = '\0';
          keylen = 0;
          vallen = 0;
          /* Insert new header (key-value pair) into request. */
          response_add_headerkv (response, key, val);
          break;
        default:
#ifdef _DEBUG
          fprintf (stderr, "Invalid header: unknown header state!\n");
#endif
          retval = -1;
          goto http_response_parse_end;
          break;
        } /* switch (headerstate) */
        headerstate = NEWKEY;
        while ((*(ptr + 1) == ' ') || (*(ptr + 1) == '\t')) ++ptr;
      } else {
#ifdef _DEBUG
        fprintf (stderr, "Invalid header: CR without LF!\n");
#endif
        retval = -1;
        goto http_response_parse_end;
      }
      break;
    case ':':
      switch (headerstate) {
      case NEWKEY:
        headerstate = NEWVAL;
        while ((*(ptr + 1) == ' ') || (*(ptr + 1) == '\t')) ++ptr;
        break;
      case NEWVAL:
        val[vallen++] = c;
        if (vallen > MAXVALLEN) {
#ifdef _DEBUG
          fprintf (stderr, "Invalid header: value too long!\n");
#endif
          retval = -1;
          goto http_response_parse_end;
        }
        break;
      default:
#ifdef _DEBUG
        fprintf (stderr, "Invalid header: unknown header state!\n");
#endif
        retval = -1;
        goto http_response_parse_end;
        break;
      } /* switch (headerstate) */
      break;
    case ' ':
    case '\t':
      switch (headerstate) {
      case NEWKEY:
        key[keylen++] = c;
        if (keylen >= MAXKEYLEN) {
#ifdef _DEBUG
          fprintf (stderr, "Invalid header: key too long!\n");
#endif
          retval = -1;
          goto http_response_parse_end;
        }
        break;
      case NEWVAL:
        val[vallen++] = c;
        if (vallen > MAXVALLEN) {
#ifdef _DEBUG
          fprintf (stderr, "Invalid header: value too long!\n");
#endif
          retval = -1;
          goto http_response_parse_end;
        }
        break;
      default:
#ifdef _DEBUG
        fprintf (stderr, "Invalid header: unknown header state!\n");
#endif
        retval = -1;
        goto http_response_parse_end;
        break;
      } /* switch (headerstate) */
      break;
    case '(':
    case ')':
    case '<':
    case '>':
    case '@':
    case ',':
    case ';':
    case '\\':
    case '\"':
    case '/':
    case '[':
    case ']':
    case '?':
    case '=':
    case '{':
    case '}':
      switch (headerstate) {
      case NEWKEY:
#ifdef _DEBUG
        fprintf (stderr, "Invalid header: illegal character in key!\n");
#endif
        retval = -1;
        goto http_response_parse_end;
      case NEWVAL:
        val[vallen++] = c;
        if (vallen > MAXVALLEN) {
#ifdef _DEBUG
          fprintf (stderr, "Invalid header: value too long!\n");
#endif
          retval = -1;
          goto http_response_parse_end;
        }
        break;
      default:
#ifdef _DEBUG
        fprintf (stderr, "Invalid header: unknown header state!\n");
#endif
        retval = -1;
        goto http_response_parse_end;
        break;
      } /* switch (headerstate) */
      break;
    case 0:                     /* NUL: Null character. '\0' */
    case 1:                     /* SOH: Start of Header. */
    case 2:                     /* STX: Start of Text. */
    case 3:                     /* ETX: End of Text. */
    case 4:                     /* EOT: End of Transmission. */
    case 5:                     /* ENQ: Enquiry. */
    case 6:                     /* ACK: Aknowledgment. */
    case 7:                     /* BEL: Bell. '\a' */
    case 8:                     /* BS: Backspace. '\b' */
    case 10:                    /* LF: Line Feed. '\n' */
    case 11:                    /* VT: Vertical Tab. '\v' */
    case 12:                    /* FF: Form Feed. '\f' */
    case 14:                    /* SO: Shift Out. */
    case 15:                    /* SI: Shift In. */
    case 16:                    /* DLE: Data Link Escape. */
    case 17:                    /* DC1: Device Control 1. */
    case 18:                    /* DC2: Device Control 2. */
    case 19:                    /* DC3: Device Control 3. */
    case 20:                    /* DC4: Device Control 4. */
    case 21:                    /* NAK: Negative Aknowledgment. */
    case 22:                    /* SYN: Synchronous Idle. */
    case 23:                    /* ETB: End of Trans the Block. */
    case 24:                    /* CAN: Cancel. */
    case 25:                    /* EM: End of Medium. */
    case 26:                    /* SUB: Substitute. */
    case 27:                    /* ESC: Escape. */
    case 28:                    /* FS: File Separator */
    case 29:                    /* GS: Group Separator. */
    case 30:                    /* RS: Record Separator. */
    case 31:                    /* US: Unit Separator. */
    case 127:
#ifdef _DEBUG
      fprintf (stderr, "Invalid header: illegal character: %x!\n", (int)c);
#endif
      retval = -1;
      goto http_response_parse_end;
      break;
    default:
      switch (headerstate) {
      case NEWKEY:
        key[keylen++] = c;
        if (keylen >= MAXKEYLEN) {
#ifdef _DEBUG
          fprintf (stderr, "Invalid header: key too long!\n");
#endif
          retval = -1;
          goto http_response_parse_end;
        }
        break;
      case NEWVAL:
        val[vallen++] = c;
        if (vallen >= MAXVALLEN) {
#ifdef _DEBUG
          fprintf (stderr, "Invalid header: value too long!\n");
#endif
          retval = -1;
          goto http_response_parse_end;
        }
        break;
      default:
#ifdef _DEBUG
        fprintf (stderr, "Invalid header: unknown header state!\n");
#endif
        retval = -1;
        goto http_response_parse_end;
        break;
      } /* switch (headerstate) */
      break;
    } /* switch (c) */
    ++ptr;
    c = *ptr;
  } /* while (1) */

 http_response_parse_end:
  if (buffer) free (buffer);

#ifdef _DEBUG
  fprintf (stderr, "http_response_parse end!\n");
#endif  /* _DEBUG */

  return retval;
}

int
http_response_check (response_t *response)
{
  header_t      *h = NULL;
  int           i = -1, cmp = 0, match = 0;
  int           retval = 0;

#ifdef _DEBUG
  fprintf (stderr, "http_response_check start!\n");
#endif

  /* Check headers. */
  list_for_each_entry(h, &response->headers, hook) {
    match = 0;
    /* response-header */
    for (i = 0; i < resphnr; ++i) {
      cmp = strcasecmp (h->key, resphn[i].name);
      if (cmp == 0) {
#ifdef _DEBUG
        fprintf (stderr, "Response: \"%s\" = \"%s\"\n", h->key, h->val);
#endif  /* _DEBUG */
        match = 1;
        switch (resphn[i].header) {
        case RESPONSE_HEADER_ACCRANGE:
          break;
        case RESPONSE_HEADER_AGE:
          break;
        case RESPONSE_HEADER_ETAG:
          /* Range = "Range" ":" ranges-specifier */
          break;
        case RESPONSE_HEADER_LOCATION:
          response_set_location (response, h->val, strlen (h->val));
          break;
        case RESPONSE_HEADER_PROXYAUTHEN:
          break;
        case RESPONSE_HEADER_RETRYAFTER:
          break;
        case RESPONSE_HEADER_SERVER:
          response_set_server (response, h->val, strlen (h->val));
          break;
        case RESPONSE_HEADER_VARY:
          break;
        case RESPONSE_HEADER_WWWAUTHEN:
          break;
        default:
          break;
        } /* switch () */
        break;
      } /* if (cmp) */
    } /* for (i) */
    if (match) continue;
    /* general-header */
    for (i = 0; i < ghnr; ++i) {
      cmp = strcasecmp (h->key, ghn[i].name);
      if (cmp == 0) {
#ifdef _DEBUG
        fprintf (stderr, "General: \"%s\" = \"%s\"\n", h->key, h->val);
#endif
        match = 1;
        switch (ghn[i].header) {
        case GENERAL_HEADER_CONNECTION:
          cmp = strcasecmp (h->val, cn[HTTP_CONNECTION_KEEPALIVE].name);
          if (cmp == 0) {
            response->keep_alive = HTTP_CONNECTION_KEEPALIVE;
          } else {
            response->keep_alive = HTTP_CONNECTION_CLOSE;
          }
          break;
        case GENERAL_HEADER_TRANSFERENC:
          cmp = strcasecmp (h->val, "chunked");
          if (cmp == 0) {
            response->chunked = 1;
          }
          break;
        default:
          break;
        } /* switch () */
        break;
      } /* if (cmp) */
    } /* for (i) */
    if (match) continue;
    /* entity-header */
    for (i = 0; i < ehnr; ++i) {
      cmp = strcasecmp (h->key, ehn[i].name);
      if (cmp == 0) {
#ifdef _DEBUG
        fprintf (stderr, "Entity: \"%s\" = \"%s\"\n", h->key, h->val);
#endif
        switch (ehn[i].header) {
        case ENTITY_HEADER_LASTMOD:
          break;
        case ENTITY_HEADER_CONLENGTH:
          response->contentlength = atol (h->val);
          break;
        case ENTITY_HEADER_CONLOC:
          break;
        default:
          break;
        } /* switch () */
        break;
      } /* if (cmp) */
    } /* for (i) */
  } /* list_for_each_entry () */

#ifdef _DEBUG
  fprintf (stderr, "http_response_check end!\n");
#endif

  return retval;
}
