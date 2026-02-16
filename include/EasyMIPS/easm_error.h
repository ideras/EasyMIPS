#ifndef __SIM_ERROR_H__
#define __SIM_ERROR_H__

#include <iostream>
#include <memory>
#include <vector>
#include <type_traits>
#include <optional>
#include "colorizer.h"

namespace EAsm
{
    enum class ErrorCode
    {
        Ok,
        Overflow,
        DivisionByZero,
        VirtualAddrOutOfRange,
        VirtualAddrNotAligned,
        InstAddrOutOfRange,
        SyscallNotImplemented,
        UnsupportedInst,
        Break,
        Stop,
        Bug,
    };

    struct SrcInfo
    {
        SrcInfo()
        : filename(), line_num(-1)
        {}

        SrcInfo(const std::string& fname, long line_num)
        : filename(fname), line_num(line_num)
        {}

        SrcInfo(const char *fname, long line_num)
        : filename(fname), line_num(line_num)
        {}

        std::string fileName() const
        { return filename; }
        
        long lineNum() const
        { return line_num; }

    private:
        std::string filename;
        long line_num;
    };

    class ErrorInfo;
    using ErrorInfoPtr = std::shared_ptr<ErrorInfo>;
    using ErrorInfoVector = std::vector<ErrorInfoPtr>;

    struct ErrorPair
    {
        ErrorPair(ErrorCode ecode, ErrorInfoPtr&& einfo)
        : err_code(ecode), err_info(std::move(einfo))
        {}

        ErrorCode err_code;
        ErrorInfoPtr err_info;
    };

    class ErrorInfo
    {
    public:
        virtual ~ErrorInfo() {}

        virtual void print(std::ostream& out) const = 0;
    };

    template <typename... TArgs>
    class ErrorInfoTuple: public ErrorInfo
    {
    public:
        ErrorInfoTuple(const ErrorInfoTuple& other)
        : tval(other.tval)
        {}

        ErrorInfoTuple(ErrorInfoTuple&& other)
        : tval(std::move(other.tval))
        {}

        ~ErrorInfoTuple()
        {}

        explicit ErrorInfoTuple(TArgs... args)
        : tval(std::forward<TArgs>(args)...)
        {}

        void print(std::ostream& out) const override
        {
            std::apply(
                [&out](const TArgs&... tmembers)
                { (out << ... << tmembers ); },
                tval
            );
        }

    private:
        std::tuple<TArgs...> tval;
    };

    class Error
    {
        template<typename T>
        using IsSrcInfoType = std::is_same<std::remove_cv_t<std::remove_reference_t<T>>, SrcInfo>;

        template<typename T>
        using IsErrorType = std::is_same<std::remove_cv_t<std::remove_reference_t<T>>, Error>;

        template<typename T>
        using IsErrorCodeType = std::is_same<std::remove_cv_t<std::remove_reference_t<T>>, ErrorCode>;

    public:
        Error(): osrc_info(), error_info()
        {}

        Error(Error&& other)
        : osrc_info(std::move(other.osrc_info)),
          error_info(std::move(other.error_info))
        {}

        Error(const Error& other)
        : osrc_info(other.osrc_info),
          error_info(other.error_info)
        {}

        template <typename... TArgs>
        explicit Error(TArgs... args)
        : osrc_info(std::nullopt), 
          error_info( new ErrorInfoTuple<TArgs...>(std::forward<TArgs>(args)...) )
        {}

        template <typename T, typename... TArgs,
                  std::enable_if_t<IsSrcInfoType<T>::value, int> = 0>
        Error(const T& arg0, TArgs... args)
        : osrc_info(arg0), 
          error_info( new ErrorInfoTuple<TArgs...>(std::forward<TArgs>(args)...) )
        {}

        bool empty() const
        { return (error_info == nullptr); }

        void operator=(Error&& rhs)
        {
            osrc_info = std::move(rhs.osrc_info);
            error_info = std::move(rhs.error_info);
        }

        void removeSrcInfo()
        { osrc_info = std::nullopt; }

        bool hasSrcInfo() const
        { return (osrc_info != std::nullopt); }

        std::string fileName() const
        { return osrc_info->fileName(); }

        long lineNum() const
        { return osrc_info->lineNum(); }

        std::optional<SrcInfo> srcInfo() const
        { return osrc_info; }

        const ErrorInfoPtr& errorInfo() const
        { return error_info; }

        void print(std::ostream& out) const
        {
            if (osrc_info)
            {
                out << colorText(fcolor::green, fileName()) << ":"
                    << colorText(fcolor::yellow, lineNum()) << ":";
            }
            if (error_info)
                error_info->print(out);
        }

    private:
        std::optional<SrcInfo> osrc_info;
        ErrorInfoPtr error_info;
    };

    static inline std::ostream& operator<<(std::ostream& out, const ErrorInfoVector& einfov)
    {
        for (const auto& einfo : einfov)
            einfo->print(out);

        return out;
    }

    static inline std::ostream& operator<<(std::ostream& out, const ErrorInfo& einfo)
    {
        einfo.print(out);
        return out;
    }

    static inline std::ostream& operator<<(std::ostream& out, const ErrorInfoPtr& einfo)
    {
        einfo->print(out);
        return out;
    }

    static inline std::ostream& operator<<(std::ostream& out, const Error& err)
    {
        err.print(out);
        return out;
    }

    template <typename... TArgs>
    static inline ErrorInfoPtr makeErrorInfo(TArgs... args)
    {   
        return std::make_shared<ErrorInfoTuple<TArgs...>>(std::forward<TArgs>(args)...);
    }

    Error errorCodeDesc(const SrcInfo& src_info, ErrorCode ecode);
    Error arithOvfError(const SrcInfo& src_info, const std::string& inst, int32_t arg1, int32_t arg2);

} // namespace EAsm

#endif
