using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;

namespace MediaStreamer.UI
{
    public static class ButtonExtension
    {
        #region CornerRadius
        public static CornerRadius GetCornerRadius(DependencyObject obj)
        {
            return (CornerRadius)obj.GetValue(CornerRadiusProperty);
        }

        public static void SetCornerRadius(DependencyObject obj, CornerRadius value)
        {
            obj.SetValue(CornerRadiusProperty, value);
        }

        public static readonly DependencyProperty CornerRadiusProperty =
            DependencyProperty.RegisterAttached("CornerRadius", typeof(CornerRadius), typeof(ButtonExtension), new FrameworkPropertyMetadata(new CornerRadius()));
        #endregion

        #region ShowSpacer
        public static bool GetShowSpacer(DependencyObject obj)
        {
            return (bool)obj.GetValue(ShowSpacerProperty);
        }

        public static void SetShowSpacer(DependencyObject obj, bool value)
        {
            obj.SetValue(ShowSpacerProperty, value);
        }

        public static readonly DependencyProperty ShowSpacerProperty =
            DependencyProperty.RegisterAttached("ShowSpacer", typeof(bool), typeof(ButtonExtension), new FrameworkPropertyMetadata(false));
        #endregion
    }
}
