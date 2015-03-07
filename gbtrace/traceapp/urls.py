from django.conf.urls import patterns, url
from traceapp import views

urlpatterns = patterns('',
    url(r'^upload/$', views.garbage_upload),
    url(r'^checkpoint/$', views.garbage_checkpoint),
    url(r'^validate/$', views.garbage_validate),
)


