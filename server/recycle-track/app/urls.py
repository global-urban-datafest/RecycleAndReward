from django.conf.urls import patterns, include, url
from django.contrib import admin
from django.conf.urls.i18n import i18n_patterns

urlpatterns = patterns('',
    # Examples:
    # url(r'^$', 'gbtrace.views.home', name='home'),
    # url(r'^blog/', include('blog.urls')),

    url(r'^admin/', include(admin.site.urls)),
    url(r'^traceapp/', include('traceapp.urls')),
)
