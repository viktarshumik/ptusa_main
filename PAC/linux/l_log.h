/// @file l_log.h
/// @brief ������ � ����������������� �������, ������ � �.�. ��� �� Linux.
///
/// @author  ������ ������� ���������.
///
/// @par ������� ������:
/// @$Rev: 332 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-08-24 09:59:40#$.

#ifndef L_LOG_H
#define L_LOG_H

#include "log.h"

#include <iostream>
#include <iomanip>
#include <ctime>

#ifndef SIMPLE_LOG
#include <syslog.h>
#endif

#include <stdio.h>

#include "l_console.h"
//-----------------------------------------------------------------------------
/// @brief ������ � ��������.
///
/// ������� ������������ ����� ��� �������.
/// �������� �������� ������ ������ - ����� � ���, �.�.
///
class l_log: public i_log
    {
    public:

    virtual ~l_log()
        {
        }

    void virtual write_log( PRIORITIES priority )
        {
#ifdef SIMPLE_LOG
        printf( "%s\n", msg );
#else
        std::time_t _tm = std::time( nullptr );
        std::tm tm = *std::localtime( &_tm );

        std::cout << std::put_time( &tm, "%Y-%m-%d %H.%M.%S " );

        switch ( priority )
            {
            case i_log::P_ALERT:
                SetColor( RED_I );
                std::cout << "ALERT  (1) -> ";
                break;

            case i_log::P_CRIT:
                SetColor( RED_I );
                std::cout << "CRITIC (2) -> ";
                break;

            case i_log::P_ERR:
                SetColor( RED );
                std::cout << "ERROR  (3) -> ";
                break;

            case i_log::P_INFO:
                SetColor( GREEN );
                std::cout << "INFO   (6) -> ";
                break;

            case i_log::P_DEBUG:
                SetColor( YELLOW );
                std::cout << "DEBUG  (7) -> ";
                break;

            default:
                break;
            }

        std::cout << msg << std::endl;
        SetColor( RESET );

        fflush( stdout );

        int len = strlen( msg ) - 1;
        for ( int i = 0; i < len; i++ )
            {
            if ( msg[ i ] == '\n' ) msg[ i ] = ';';
            }

        //Using openlog() because kbus functions also use it
        //(openlog( "kbus", ...)).
        openlog( "ptusa", LOG_PID, LOG_USER );

        syslog( (int) priority, "%s", msg );
        closelog();
#endif
        }

    l_log()
        {
        }
};

#endif // L_LOG_H
