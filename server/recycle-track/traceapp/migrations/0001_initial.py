# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations
from django.conf import settings


class Migration(migrations.Migration):

    dependencies = [
        migrations.swappable_dependency(settings.AUTH_USER_MODEL),
    ]

    operations = [
        migrations.CreateModel(
            name='Bag',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('barcode', models.CharField(max_length=50)),
                ('user', models.ForeignKey(related_name='BagUser', to=settings.AUTH_USER_MODEL, null=True)),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='BagTrace',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('event', models.PositiveIntegerField(choices=[(0, 'Assign to user'), (1, 'Dump bag'), (2, 'Bag accepted'), (3, 'Bag rejected'), (4, 'Bag arrived'), (5, 'Bag passed'), (6, 'Bag failed')])),
                ('time', models.DateTimeField()),
                ('extra', models.CharField(max_length=50, null=True)),
                ('bag', models.ForeignKey(related_name='BagTraceBag+', to='traceapp.Bag')),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='CheckPoint',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('name', models.CharField(max_length=100)),
                ('key', models.CharField(max_length=255)),
                ('point_type', models.IntegerField()),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.CreateModel(
            name='GarbageTraceProfile',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('address', models.CharField(default=b'', max_length=255, verbose_name='Address')),
                ('points', models.IntegerField(default=0, verbose_name='Points')),
                ('points_onway', models.IntegerField(default=0, verbose_name='Points on the way')),
                ('points_failed', models.IntegerField(default=0, verbose_name='Points failed')),
                ('total_weight', models.FloatField(default=0, verbose_name='Total Weight')),
                ('user', models.OneToOneField(to=settings.AUTH_USER_MODEL)),
            ],
            options={
            },
            bases=(models.Model,),
        ),
        migrations.AddField(
            model_name='bagtrace',
            name='check_point',
            field=models.ForeignKey(related_name='BagTraceCheckPoint+', to='traceapp.CheckPoint'),
            preserve_default=True,
        ),
        migrations.AddField(
            model_name='bagtrace',
            name='user',
            field=models.ForeignKey(related_name='BagTraceUser+', to=settings.AUTH_USER_MODEL),
            preserve_default=True,
        ),
    ]
