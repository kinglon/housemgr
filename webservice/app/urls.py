from django.urls import path
from . import houseviews
from . import imageviews


urlpatterns = [
    path('house', houseviews.create_house),
    path('house/<int:house_id>', houseviews.handle_house),
    path('house/list', houseviews.list_house),
    path('image', imageviews.create_image),
    path('image/<str:image_id>', imageviews.get_image),
]
