//******************************************************************
//
// Copyright 2016 Intel Corporation All Rights Reserved.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#ifndef UPNP_EXCEPTION_H_
#define UPNP_EXCEPTION_H_

#include <exception>
#include <string>

typedef enum
{
    UPNP_OK = 0,
    UPNP_ERROR_NOT_IMPLEMENTED = 1,
    UPNP_ERROR_BAD_URI = 2,
} UpnpErrorCode;

class UpnpException : public std::exception
{
    public:
        UpnpException(UpnpErrorCode errorCode, const std::string &msg);

        virtual const char *what() const noexcept;

        virtual UpnpErrorCode code() const;
        virtual ~UpnpException() throw() {}

    private:
        UpnpErrorCode m_errorCode;
        std::string m_msg;
};

class NotImplementedException : public UpnpException
{
    public:
        NotImplementedException(const std::string &msg);
};

class BadUriException : public UpnpException
{
    public:
        BadUriException(const std::string &msg);
};
#endif
