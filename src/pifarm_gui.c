#include <pifarm_gui.h>

extern context_t     *      p_ctx ;
extern uint8_t              REFRESH_1HZ  ;
extern uint8_t              REFRESH_10HZ ;

void debug_draw_click_zone(Ez_event *ev, uint8_t * scale_factor )
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( scale_factor == NULL );

    uint8_t i ;
    click_zone_t *t[7] = { p_ctx->gc->cz_btn_play, p_ctx->gc->cz_btn_pause, \
        p_ctx->gc->cz_btn_stop, p_ctx->gc->cz_btn_light, p_ctx->gc->cz_btn_fan, \
        p_ctx->gc->cz_btn_heat, p_ctx->gc->cz_btn_water  } ;

    ez_set_color (RED) ;
    ez_draw_text (ev->win, EZ_TL, DEFAULT_WINDOW_WIDTH -120 , DEFAULT_WINDOW_HEIGHT -20, "debug click zone ON" );
    if ( REFRESH_1HZ > 50 )
    {
        for (i=0; i<7; i++)
        {
            ez_set_thick(2) ;
            ez_draw_rectangle (
                ev->win,
                *scale_factor * t[i]->x1 ,
                *scale_factor * t[i]->y1,
                *scale_factor * t[i]->x2,
                *scale_factor * t[i]->y2 );
            ez_set_thick(1) ;
           ez_draw_text (ev->win, EZ_TL, t[i]->x1 + 4, t[i]->y1 +2, t[i]->desc);
        }
    }

}

uint8_t click_match(Ez_event *ev, click_zone_t * zone)
{
    DEBUG_ASSERT( ev == NULL );
    DEBUG_ASSERT( zone == NULL );

    if ( (ev->mx <= zone->x1) || ( ev->mx >= zone->x2 ) ) return 1 ;
    if ( (ev->my <= zone->y1) || ( ev->my >= zone->y2 ) ) return 1 ;
    return 0 ;
}

void win_on_button_press(Ez_event *ev)                /* Mouse button pressed */
{
    DEBUG_ASSERT( ev == NULL );

#ifdef DBG_GUI_EVENTS
    printf ("DBG [GUI EVENTS]    - ButtonPress     win = 0x%x  mx = %d  my = %d  mb = %d\n",
        ez_window_get_id(ev->win), ev->mx, ev->my, ev->mb);
#endif
    if (ev->mb == 1)
    {
        /* PLAY */
        if ( 0 == click_match(ev, p_ctx->gc->cz_btn_play ) )
        {
            command_function("LED_DRIVER",  OFF);
            command_function("FAN_DRIVER",  OFF);
            command_function("PUMP_DRIVER", OFF);
            command_function("HEAT_DRIVER", OFF);
            p_ctx->mode = AUTO ;
#ifdef DBG_LOGIC
            printf("DBG [LOGIC EVENT]   - Play request, previous p_ctx->running_status %d\n", p_ctx->running_status);
#endif
            if (( p_ctx->running_status == STATE_STOPPED ) || ( p_ctx->running_status == STATE_PAUSED )) { p_ctx->running_status = STATE_RUNNING ;}
#ifdef DBG_LOGIC
            printf("DBG [LOGIC EVENT]   - Play request done, p_ctx->running_status %d\n", p_ctx->running_status);
#endif
        }

        /* PAUSE */
        if ( 0 == click_match(ev, p_ctx->gc->cz_btn_pause ) )
        {
#ifdef DBG_LOGIC
            printf("DBG [LOGIC EVENT]   - Pause request, previous p_ctx->running_status %d\n", p_ctx->running_status);
#endif
            if ( p_ctx->running_status == STATE_RUNNING ) { p_ctx->running_status = STATE_PAUSED ;}
#ifdef DBG_LOGIC
            printf("DBG [LOGIC EVENT]   - Pause request done, p_ctx->running_status %d\n", p_ctx->running_status);
#endif
        }

        /* STOP */
        if ( 0 == click_match(ev, p_ctx->gc->cz_btn_stop  ) )
        {
            command_function("LED_DRIVER",  OFF);
            command_function("FAN_DRIVER",  OFF);
            command_function("PUMP_DRIVER", OFF);
            command_function("HEAT_DRIVER", OFF);
            p_ctx->mode = NONE ;
#ifdef DBG_LOGIC
            printf("DBG [LOGIC EVENT]   - Stop request, previous p_ctx->running_status %d\n", p_ctx->running_status);
#endif
            if ( p_ctx->running_status != STATE_STOPPED ) { p_ctx->running_status = STATE_STOPPED ;}
#ifdef DBG_LOGIC
            printf("DBG [LOGIC EVENT]   - Stop request done, p_ctx->running_status %d\n", p_ctx->running_status);
#endif
        }

        /* LIGHT */
        if ( 0 == click_match(ev, p_ctx->gc->cz_btn_light ) )
        {
            p_ctx->mode = MANUAL ;
            p_ctx->running_status = STATE_STOPPED ;
#ifdef DBG_LOGIC
            printf("DBG [LOGIC EVENT]   - Growing lights toggle request, previous state p_ctx->light_status %d\n", p_ctx->light_status);
#endif
            if ( p_ctx->light_status == OFF ) { p_ctx->light_status = ON;} else {p_ctx->light_status = OFF ;}
            command_function("LED_DRIVER", p_ctx->light_status  );
            //command_light(p_ctx->light_status);
#ifdef DBG_LOGIC
            printf("DBG [LOGIC EVENT]   - Growing lights toggle request done, p_ctx->light_status %d\n", p_ctx->light_status);
#endif
        }

        /* WATER */
        if ( 0 == click_match(ev, p_ctx->gc->cz_btn_water ) )
        {
            p_ctx->mode = MANUAL ;
            p_ctx->running_status = STATE_STOPPED ;
#ifdef DBG_LOGIC
            printf("DBG [LOGIC EVENT]   - Watering toggle request, previous state p_ctx->water_status %d\n", p_ctx->water_status);
#endif
            if ( p_ctx->water_status == OFF ) { p_ctx->water_status = ON;} else {p_ctx->water_status = OFF ;}
            command_function("PUMP_DRIVER", p_ctx->water_status );
#ifdef DBG_LOGIC
            printf("DBG [LOGIC EVENT]   - Watering toggle request done, p_ctx->water_status %d\n", p_ctx->water_status);
#endif
        }

        /* FAN */
        if ( 0 == click_match(ev, p_ctx->gc->cz_btn_fan ) )
        {
            p_ctx->mode = MANUAL ;
            p_ctx->running_status = STATE_STOPPED ;
#ifdef DBG_LOGIC
            printf("DBG [LOGIC EVENT]   - Wind toggle request, previous state p_ctx->fan_status %d\n", p_ctx->fan_status);
#endif
            if ( p_ctx->fan_status == OFF ) { p_ctx->fan_status = ON;} else {p_ctx->fan_status = OFF ;}
            command_function("FAN_DRIVER", p_ctx->fan_status );
#ifdef DBG_LOGIC
            printf("DBG [LOGIC EVENT]   - Wind toggle request done, p_ctx->fan_status %d\n", p_ctx->fan_status);
#endif
        }

        /* HEAT */
        if ( 0 == click_match(ev, p_ctx->gc->cz_btn_heat ) )
        {
            p_ctx->mode = MANUAL ;
            p_ctx->running_status = STATE_STOPPED ;
#ifdef DBG_LOGIC
            printf("DBG [LOGIC EVENT]   - Heater toggle request, previous state p_ctx->heat_status %d\n", p_ctx->heat_status);
#endif
            if ( p_ctx->heat_status == OFF ) { p_ctx->heat_status = ON;} else {p_ctx->heat_status = OFF ;}
            command_function("HEAT_DRIVER",  p_ctx->heat_status);
#ifdef DBG_LOGIC
            printf("DBG [LOGIC EVENT]   - Heater toggle request done, p_ctx->heat_status %d\n", p_ctx->fan_status);
#endif
        }
    }
}


void win_on_button_release(Ez_event *ev)             /* Mouse button released */
{
    DEBUG_ASSERT( ev == NULL );

#ifdef DBG_GUI_EVENTS
    printf ("DBG [GUI EVENTS]    - ButtonRelease   win = 0x%x  mx = %d  my = %d  mb = %d\n",
        ez_window_get_id(ev->win), ev->mx, ev->my, ev->mb);
#endif
}

void win_on_motion_notify(Ez_event *ev)                        /* Mouse moved */
{
    DEBUG_ASSERT( ev == NULL );

#ifdef DBG_GUI_EVENTS
    printf ("DBG [GUI EVENTS]    - MotionNotify    win = 0x%x  mx = %d  my = %d  mb = %d\n",
        ez_window_get_id(ev->win), ev->mx, ev->my, ev->mb);
#endif
}

void win_on_timer_notify(Ez_event *ev)               /* The timer has expired */
{
    DEBUG_ASSERT( ev == NULL );

#ifdef DBG_GUI_EVENTS
    printf ("DBG [GUI EVENTS]    - TimerNotify     win = 0x%x\n", ez_window_get_id(ev->win));
#endif
    ez_send_expose (ev->win);
    ez_start_timer (ev->win, REFRESH_INTERVAL);

    REFRESH_1HZ   +=1 ;
    REFRESH_10HZ  +=1 ;

    if (( REFRESH_1HZ == 100 ) && ( p_ctx->mode == AUTO )) auto_program();

    /* TEST RPH & REFRESH */
    if ( REFRESH_1HZ == 100 ) acquire_bme280();

    /* INIT CMP IF NEEDED */
    if ( REFRESH_1HZ == 100 )  REFRESH_1HZ   = 0 ;
    if ( REFRESH_10HZ == 10 )  REFRESH_10HZ  = 0 ;

}


void win_on_key_press(Ez_event *ev)                            /* Key pressed */
{
    DEBUG_ASSERT( ev == NULL );

#ifdef DBG_GUI_EVENTS
    printf ("DBG [GUI EVENTS]    - KeyPress        win = 0x%x  mx = %d  my = %d  "
            "key_sym = 0x%x  key_name = %s  key_count = %d  key_string = \"%s\"\n",
        ez_window_get_id(ev->win), ev->mx, ev->my,
        (int) ev->key_sym, ev->key_name, ev->key_count,
        ev->key_sym == XK_Return || ev->key_sym == XK_KP_Enter ? "" : ev->key_string);
#endif

    switch (ev->key_sym)
    {
        case XK_q     : ez_quit (); break;
        case XK_space : ez_send_expose (ev->win); break;
        default : break ;
    }
}

void win_on_key_release(Ez_event *ev)                         /* Key released */
{
    DEBUG_ASSERT( ev == NULL );

#ifdef DBG_GUI_EVENTS
    printf ("DBG [GUI EVENTS]    - KeyRelease      win = 0x%x  mx = %d  my = %d  "
            "key_sym = 0x%x  key_name = %s  key_count = %d  key_string = \"%s\"\n",
        ez_window_get_id(ev->win), ev->mx, ev->my,
        (int) ev->key_sym, ev->key_name, ev->key_count,
        ev->key_sym == XK_Return || ev->key_sym == XK_KP_Enter ? "" : ev->key_string);
#endif
     switch (ev->key_sym)
     {
        case XK_q : ez_quit (); break;
        default : break ;
    }
}

void win_on_configure_notify(Ez_event *ev)             /* Window size changed */
{
    DEBUG_ASSERT( ev == NULL );

#ifdef DBG_GUI_EVENTS
    printf ("DBG [GUI EVENTS]    - ConfigureNotify win = 0x%x  width = %d  height = %d\n",
        ez_window_get_id(ev->win), ev->width, ev->height);
#endif
}

void win_on_window_close(Ez_event *ev)                /* Close button pressed */
{
    DEBUG_ASSERT( ev == NULL );

#ifdef DBG_GUI_EVENTS
    printf ("DBG [GUI EVENTS]    - WindowClose     win = 0x%x\n", ez_window_get_id(ev->win));
#endif
}

void win_on_expose(Ez_event *ev)                 /* We must redraw everything */
{
    DEBUG_ASSERT( ev == NULL );

    uint8_t                     scale_factor ;
    static char *               p_segment_data ;
    uint8_t                     segment_size ;

    static widget_position_t    segment_position ;
    static widget_position_t    clock_position ;
    static widget_position_t    controlpanel_position;
    static widget_position_t    relay_position ;


#ifdef DBG_GUI_EVENTS
    printf ("DBG [GUI EVENTS]    - Expose          win = 0x%x\n", ez_window_get_id(ev->win));
#endif

    scale_factor = 1 ;

    /* SET BACKGROUND */
    ez_set_color (DEFAULT_BACKGROUND_COLOR);
    ez_fill_rectangle (
        ev->win,
        0,
        0 ,
        DEFAULT_WINDOW_WIDTH ,
        DEFAULT_WINDOW_HEIGHT);

    /* CLOCK WIDGET */

    clock_position.x   = 10 ;
    clock_position.y   = 10 ;

    draw_clock_widget(ev, &scale_factor, &clock_position ) ;

    /* TEMPERATURE WIDGET */
    segment_position.x   = 10 ;
    segment_position.y   = 80 ;

    p_segment_data = malloc(10 * sizeof(char)) ;
    DEBUG_ASSERT( p_segment_data == NULL);

    float_to_string(p_ctx->temperature, p_segment_data, 2);
    segment_size = strlen(p_segment_data) ;

    draw_temperature_widget(
        ev,
        &scale_factor,
        &segment_position,
        segment_size,
        p_segment_data
        ) ;

    /* HYDRO WIDGET */
    segment_position.x   = 150 ;
    segment_position.y   = 80 ;

    float_to_string(p_ctx->humidity, p_segment_data, 2);

    segment_size = strlen(p_segment_data) ;

    draw_humidity_widget(
        ev,
        &scale_factor,
        &segment_position,
        segment_size,
        p_segment_data
        ) ;

    /* PRESSURE WIDGET */
    segment_position.x   = 10 ;
    segment_position.y   = 150 ;

    float_to_string(p_ctx->pressure, p_segment_data, 0);

    segment_size = strlen(p_segment_data) ;

    draw_pressure_widget(
        ev,
        &scale_factor,
        &segment_position,
        segment_size,
        p_segment_data
        ) ;

    /* ALTITUDE WIDGET */
    segment_position.x   = 150 ;
    segment_position.y   = 150 ;

    sprintf(p_segment_data, "%4d", (int)p_ctx->altitude) ;

    draw_altitude_widget(
        ev,
        &scale_factor,
        &segment_position,
        4,
        p_segment_data
        ) ;

    /* CONTOL PANEL WIDGET */

    controlpanel_position.x = 305 ;
    controlpanel_position.y = 10 ;

    draw_controlpanel_widget(ev, &scale_factor, &controlpanel_position) ;

    /* RELAY WIDGET */
    relay_position.x   = 10 ;
    relay_position.y   = 220 ;

    draw_relays_widget(
        ev,
        &relay_position ) ;

    free(p_segment_data) ;
    p_segment_data = NULL ;

#ifdef DBG_GUI_CLICK_ZONES
    debug_draw_click_zone(ev, &scale_factor) ;
#endif
}

void win_on_event(Ez_event *ev)
{
    DEBUG_ASSERT( ev == NULL );

    switch (ev->type)
    {
        case Expose          : win_on_expose           (ev); break;
        case ButtonPress     : win_on_button_press     (ev); break;
        case ButtonRelease   : win_on_button_release   (ev); break;
        case MotionNotify    : win_on_motion_notify    (ev); break;
        case KeyPress        : win_on_key_press        (ev); break;
        case KeyRelease      : win_on_key_release      (ev); break;
        case ConfigureNotify : win_on_configure_notify (ev); break;
        case WindowClose     : win_on_window_close     (ev); break;
        case TimerNotify     : win_on_timer_notify     (ev); break;
        default :
#ifdef DBG_GUI_EVENTS
             printf ("DBG [CONFIG PARSER] - Unknown event: %d\n", ev->type);
#endif
             break ;
    }
}
